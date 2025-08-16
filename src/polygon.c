#include "polygon.h"
#include "objects.h"

#define POLYGON_MAX_SHAPES 16

int needed_verts(int n_points) {
    int count = n_points;
    while (count > B2_MAX_POLYGON_VERTICES) {
        n_points += 2;
        count -= B2_MAX_POLYGON_VERTICES - 2;
    }
    return n_points;
}

// This is simplified version of b2ComputePolygonMass that works for more than B2_MAX_POLYGON_VERTICES points.
b2Vec2 get_center_of_mass( const b2Vec2* points, int count, float density) {
    b2Vec2 center = { 0.0f, 0.0f };
    float area = 0.0f;
    

    // Get a reference point for forming triangles.
    // Use the first vertex to reduce round-off errors.
    b2Vec2 r = points[0];

    const float inv3 = 1.0f / 3.0f;

    for ( int i = 1; i < count - 1; ++i) {
            // Triangle edges
            b2Vec2 e1 = b2Sub(points[i], r );
            b2Vec2 e2 = b2Sub(points[i + 1], r);

            float D = b2Cross( e1, e2 );

            float triangleArea = 0.5f * D;
            area += triangleArea;

            // Area weighted centroid, r at origin
            center = b2MulAdd( center, triangleArea * inv3, b2Add( e1, e2 ) );
    }
    // Center of mass, shift back from origin at r
    float invArea = 1.0f / area;
    center.x = center.x * invArea + r.x;
    center.y = center.y * invArea + r.y;
    return center;
}


Polygon* Polygon_create(b2WorldId world, const b2Vec2* points, int n_points, b2Transform t) {
    if (n_points >= INT16_MAX) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Tried to create polygon with more than INT16_MAX vertices");
        return NULL;
    }
    b2Vec2 center = get_center_of_mass(points, n_points, 1.0f);
    Polygon* poly = SDL_malloc(sizeof(Polygon) + n_points * sizeof(b2Vec2));
    SDL_assert_release(poly != NULL);
    SDL_memcpy(poly->points, points, n_points * sizeof(b2Vec2));

    for (int i = 0; i < n_points; ++i) {
        poly->points[i].x -= center.x;
        poly->points[i].y -= center.y;
    }

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = t.p;
    bodyDef.linearDamping = 0.2f;
    bodyDef.angularDamping = 0.2f;
    b2Vec2 wold_center = b2RotateVector(t.q, center);

    bodyDef.position.x += wold_center.x;
    bodyDef.position.y += wold_center.y;
    bodyDef.rotation = t.q;
    b2BodyId body = b2CreateBody(world, &bodyDef);
    poly->count = n_points;

    int total_verts = 0;
    if (n_points <= B2_MAX_POLYGON_VERTICES) {
        b2Hull hull = b2ComputeHull(poly->points, n_points);
        total_verts += hull.count;
        if (hull.count > 0) {
            // b2ComputeHull may have simplified polygon
            for (int i = 0; i < hull.count; ++i) {
                poly->points[i] = hull.points[i];
            }
            poly->count = hull.count;

            b2Polygon dynPoly = b2MakePolygon(&hull, 0.0f);
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.filter.categoryBits = OBJECT_TYPE_POLYGON;
            shapeDef.density = 1.0f;
            shapeDef.userData = poly;
            shapeDef.enableSensorEvents = true;
            b2CreatePolygonShape(body, &shapeDef, &dynPoly);
        } else {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "b2ComputeHull return 0 vertices");
        }
    } else {
        // Idealy we would simplify poly->points to match hull,
        // But this is not trivial...
        // Maybe increase B2_MAX_POLYGON_VERTICES instead?

        int tot_verts = needed_verts(n_points);
        int parts = SDL_ceil((double)tot_verts / (double) B2_MAX_POLYGON_VERTICES);

        while (parts > POLYGON_MAX_SHAPES) {
            poly->count -= 1;
            tot_verts = needed_verts(poly->count);
            parts = SDL_ceil((double)tot_verts / (double) B2_MAX_POLYGON_VERTICES);
        }
        if (poly->count < n_points) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Reduced number of verticies due to POLYGON_MAX_SHAPES limit");
        }

        int verts_per = tot_verts / parts;
        int ix = 1;
        for (int i = 0; i < parts; ++i) {
            b2Vec2 verts[B2_MAX_POLYGON_VERTICES];
            verts[0] = poly->points[0];
            int part_count = verts_per;
            if (i == parts - 1) {
                part_count = tot_verts - verts_per * (parts - 1);
            }
            for (int j = 1; j < part_count; ++j) {
                verts[j] = poly->points[ix];
                ++ix;
            }
            --ix;
            b2Hull hull = b2ComputeHull(verts, part_count);
            total_verts += hull.count;
            if (hull.count > 0) {
                SDL_assert_release(hull.count > 0);
                b2Polygon dynPoly = b2MakePolygon(&hull, 0.0f);
                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.filter.categoryBits = OBJECT_TYPE_POLYGON;
                shapeDef.density = 1.0f;
                shapeDef.userData = poly;
                shapeDef.enableSensorEvents = true;
                b2CreatePolygonShape(body, &shapeDef, &dynPoly);
            } else {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "b2ComputeHull return 0 vertices");
            }
        }
    }

    if (total_verts == 0) {
        SDL_free(poly);
        b2DestroyBody(body);
        return NULL;
    }

    poly->body = body;

    return poly;
}


void Polygon_free(Polygon* poly) {
    b2DestroyBody(poly->body);
    SDL_free(poly);
}

