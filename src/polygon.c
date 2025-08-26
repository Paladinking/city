#include "polygon.h"
#include "objects.h"
#include "list.h"
#include <stdio.h>

#define POLYGON_MAX_SHAPES 128

bool is_clockwise(b2Vec2 v1, b2Vec2 v2, b2Vec2 v3) {
    return (-v2.y * v3.x + v1.y * (-v2.x + v3.x) + v1.x * (v2.y - v3.y) + v2.x * v3.y) > 0;
}


int needed_verts(int n_points) {
    int count = n_points;
    while (count > B2_MAX_POLYGON_VERTICES) {
        n_points += 2;
        count -= B2_MAX_POLYGON_VERTICES - 2;
    }
    return n_points;
}

b2Vec2 inner_vec(b2Vec2 prev, b2Vec2 cur, b2Vec2 next, float width) {
    b2Vec2 v1 = b2Normalize(b2Sub(cur, prev));
    b2Vec2 v2 = b2Normalize(b2Sub(next, cur));

    b2Vec2 n1 = b2LeftPerp(v1);
    b2Vec2 n2 = b2LeftPerp(v2);

    b2Vec2 bisect = b2Normalize(b2Add(n1, n2));
    float scale = 1.0f / b2Dot(bisect, n1);

    float mlen = width * scale;

    bisect = b2MulSV(mlen, bisect);

    return b2Add(cur, bisect);
}

void indented_polygon(const b2Vec2* points, uint32_t count, float width, b2Vec2* dest) {
    SDL_assert_release(count >= 3);
    b2Vec2 prev = points[count - 2];
    b2Vec2 cur = points[count - 1];
    b2Vec2 next = points[0];

    dest[count - 1] = inner_vec(prev, cur, next, width);

    prev = points[count - 1];
    cur = points[0];
    next = points[1];
    dest[0] = inner_vec(prev, cur, next, width);
    for (uint32_t i = 1; i < count - 1; ++i) {
        prev = points[i - 1];
        cur = points[i];
        next = points[i + 1];
        dest[i] = inner_vec(prev, cur, next, width);
    }
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


Collider* Polygon_create(b2WorldId world, const b2Vec2* points, int n_points, b2Transform t) {
    if (n_points >= INT16_MAX) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Tried to create polygon with more than INT16_MAX vertices");
        return NULL;
    }
    b2Vec2 center = get_center_of_mass(points, n_points, 1.0f);
    Collider* poly = SDL_malloc(sizeof(Collider) + n_points * sizeof(b2Vec2));
    SDL_assert_release(poly != NULL);
    SDL_memcpy(poly->points, points, n_points * sizeof(b2Vec2));

    for (int i = 0; i < n_points; ++i) {
        poly->points[i].x -= center.x;
        poly->points[i].y -= center.y;
    }

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;
    bodyDef.position = t.p;
    bodyDef.linearDamping = 0.0f;
    bodyDef.angularDamping = 0.0f;
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


void Polygon_free(Collider* poly) {
    b2DestroyBody(poly->body);
    SDL_free(poly);
}

bool line_intersects(b2Vec2 pa1, b2Vec2 pa2, b2Vec2 pb1, b2Vec2 pb2, b2Vec2* intersect) {
    double denom = (pa1.x - pa2.x) * (pb1.y - pb2.y) - (pa1.y - pa2.y) * (pb1.x - pb2.x);
    if (denom == 0.0) {
        return false;
    }

    double t = ((pa1.x - pb1.x) * (pb1.y - pb2.y) - (pa1.y - pb1.y) * (pb1.x - pb2.x)) / denom;
    double u = -((pa1.x - pa2.x) * (pa1.y - pb1.y) - (pa1.y - pa2.y) * (pa1.x - pb1.x)) / denom;
    intersect->x = pa1.x + t * (pa2.x - pa1.x);
    intersect->y = pa1.y + t * (pa2.y - pa1.y);
    return true;
}


bool line_segments_intersects(b2Vec2 pa1, b2Vec2 pa2, b2Vec2 pb1, b2Vec2 pb2, b2Vec2* intersect) {
    double denom = (pa1.x - pa2.x) * (pb1.y - pb2.y) - (pa1.y - pa2.y) * (pb1.x - pb2.x);
    if (denom == 0.0) {
        return false;
    }

    double t = ((pa1.x - pb1.x) * (pb1.y - pb2.y) - (pa1.y - pb1.y) * (pb1.x - pb2.x)) / denom;
    double u = -((pa1.x - pa2.x) * (pa1.y - pb1.y) - (pa1.y - pa2.y) * (pa1.x - pb1.x)) / denom;
    if (t < 0 || t > 1 || u < 0 || u > 1) {
        return false;
    }
    intersect->x = pa1.x + t * (pa2.x - pa1.x);
    intersect->y = pa1.y + t * (pa2.y - pa1.y);
    return true;
}

bool triangle_contains_point2(Triangle t, b2Vec2 p) {
    if (b2DistanceSquared(t.p1, p) < 0.01f) {
        return false;
    }
    if (b2DistanceSquared(t.p2, p) < 0.01f) {
        return false;
    }
    if (b2DistanceSquared(t.p3, p) < 0.01f) {
        return false;
    }

    b2Vec2 v0 = b2Sub(t.p3, t.p1);
    b2Vec2 v1 = b2Sub(t.p2, t.p1);
    b2Vec2 v2 = b2Sub(p, t.p1);

    float dot00 = b2Dot(v0, v0);
    float dot01 = b2Dot(v0, v1);
    float dot02 = b2Dot(v0, v2);
    float dot11 = b2Dot(v1, v1);
    float dot12 = b2Dot(v1, v2);

    float denom = dot00 * dot11 - dot01 * dot01;
    if (denom == 0.0f) {
        return false;
    }
    float inv_denom = 1.0f / denom;
    float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
    float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

    if (!((u >= 0) && (v >= 0) && (u + v <= 1))) {
        return false;
    }
    return true;
}

bool triangle_contains_point(Triangle t, b2Vec2 p) {
    float area2 = (-t.p2.y * t.p3.x + t.p1.y * (-t.p2.x + t.p3.x) + 
                   t.p1.x * (t.p2.y - t.p3.y) + t.p2.x * t.p3.y);
    float s1, s2, s3, t1, t2, t3;
    if (area2 < 0) {
        area2 *= -1;
        s1 = t.p1.x * t.p3.y - t.p1.y * t.p3.x;
        s2 = t.p1.y - t.p3.y;
        s3 = t.p3.x - t.p1.x;
        t1 = t.p1.y * t.p2.x - t.p1.x * t.p2.y;
        t2 = t.p2.y - t.p1.y;
        t3 = t.p1.x - t.p2.x;
    } else {
        s1 = t.p1.y * t.p3.x - t.p1.x * t.p3.y;
        s2 = t.p3.y - t.p1.y;
        s3 = t.p1.x - t.p3.x;
        t1 = t.p1.x * t.p2.y - t.p1.y * t.p2.x;
        t2 = t.p1.y - t.p2.y;
        t3 = t.p2.x - t.p1.x;
    }
    float sp = s1 + s2 * p.x + s3 * p.y;
    float tp = t1 + t2 * p.x + t3 * p.y;
    return sp > 0.0 && tp > 0.0 && area2 > sp + tp;
}

bool contains_point(b2Vec2* points, uint32_t count, b2Vec2 p) {
    if (count < 3) {
        return false;
    }

    bool inside = false;
    for (int i = 0, j = count - 1; i < count; j = i++) {
        b2Vec2 pi = points[i];
        b2Vec2 pj = points[j];

        if (((pi.y > p.y) != (pj.y > p.y)) &&
            (p.x < (pj.x - pi.x) * (p.y - pi.y) / (pj.y - pi.y) + pi.x)) {
            inside = !inside;
        }
    }

    return inside;
}



Triangle* lines_to_triangles(const b2Vec2* points, uint32_t size, uint32_t* out_size) {
    b2Vec2* work = SDL_malloc(size * sizeof(b2Vec2));
    uint32_t base_size = size;
    SDL_assert_release(work != NULL);
    memcpy(work, points, size * sizeof(b2Vec2));

    Triangle* dest;
    Storage store;
    LIST_CREATE(dest, store, Triangle);
    *out_size = 0;
    if (size < 3) {
        return NULL;
    }

    while (1) {
loop:
        if (size == 3) {
            const Triangle t = {work[0], work[1], work[2]};
            LIST_APPEND(dest, store, t);
            *out_size = store.size;
            return dest;
        }
        for (uint32_t ix = 0; ix < size; ++ix) {
            b2Vec2 p1 = work[(ix + size - 1) % size];
            b2Vec2 p2 = work[ix];
            b2Vec2 p3 = work[(ix + 1) % size];
            Triangle t = {p1, p2, p3};

            b2Vec2 mid = {p1.x + (p3.x - p1.x) / 2.0f, p1.y + (p3.y - p1.y) / 2.0f};
            if (!contains_point(work, size, mid)) {
                continue;
            }

            bool intersects = false;
            for (uint32_t j = 0; j < base_size; ++j) {
                b2Vec2 p = points[j];
                if (triangle_contains_point2(t, p)) {
                    intersects = true;
                    break;
                }
            }
            if (intersects) {
                continue;
            }
            LIST_APPEND(dest, store, t);
            --size;
            memmove(work + ix, work + ix + 1, (size - ix) * sizeof(b2Vec2));
            goto loop;
        }
        printf("Pos: (%f, %f)\n", work[0].x, work[0].y);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not convert lines to triangles");
        *out_size = store.size;
        return dest;
    }
}

struct wPoly {
    b2Vec2* points;
    Storage store;
};


bool add_triangle(struct wPoly* poly, Triangle t) {
    if (poly->store.size >= B2_MAX_POLYGON_VERTICES) {
        return false;
    }

    b2Vec2 v1 = poly->points[poly->store.size - 1];
    b2Vec2 v2;
    for (uint32_t i = 0; i < poly->store.size - 1; ++i, v1 = v2) {
        v2 = poly->points[i];
        b2Vec2 n;
        if (v1.x == t.p1.x && v1.y == t.p1.y) {
            if (v2.x == t.p2.x && v2.y == t.p2.y) {
                n = t.p3;
            } else if (v2.x == t.p3.x && v2.y == t.p3.y) {
                n = t.p2;
            } else {
                continue;
            }
        } else if (v1.x == t.p2.x && v1.y == t.p2.y) {
            if (v2.x == t.p1.x && v2.y == t.p1.y) {
                n = t.p3;
            } else if (v2.x == t.p3.x && v2.y == t.p3.y) {
                n = t.p1;
            } else {
                continue;
            }
        } else if (v1.x == t.p3.x && v1.y == t.p3.y) {
            if (v2.x == t.p1.x && v2.y == t.p1.y) {
                n = t.p2;
            } else if (v2.x == t.p2.x && v2.y == t.p2.y) {
                n = t.p1;
            } else {
                continue;
            }
        } else {
            continue;
        }
        b2Vec2 v0 = poly->points[(i + poly->store.size - 2) % poly->store.size];
        if (is_clockwise(v0, v1, v2) != is_clockwise(v0, v1, n)) {
            return false;
        }
        b2Vec2 v3 = poly->points[i + 1];
        if (is_clockwise(v1, v2, v3) != is_clockwise(n, v2, v3)) {
            return false;
        }
        LIST_APPEND(poly->points, poly->store, v2);
        memmove(poly->points + i + 1, poly->points + i,
                (poly->store.size - i - 1) * sizeof(b2Vec2));
        poly->points[i] = n;
        return true;
    }
    return false;
}

extern Polygon* triangles_to_polygons(const Triangle* triangles, uint32_t size, uint32_t* out_size) {
    struct wPoly* polys;
    Storage poly_store;
    LIST_CREATE(polys, poly_store, struct wPoly);

    for (uint32_t ix = 0; ix < size; ++ix) {
        uint32_t j = 0;
        for (; j < poly_store.size; ++j) {
            if (add_triangle(polys + j, triangles[ix])) {
                break;
            }
        }
        if (j == poly_store.size) {
            struct wPoly p;
            LIST_CREATE(p.points, p.store, b2Vec2);
            LIST_APPEND(p.points, p.store,triangles[ix].p1);
            LIST_APPEND(p.points, p.store,triangles[ix].p2);
            LIST_APPEND(p.points, p.store,triangles[ix].p3);
            LIST_APPEND(polys, poly_store, p);
        }
    }

    Polygon* dest = SDL_malloc(poly_store.size * sizeof(Polygon));
    SDL_assert_release(dest != NULL);
    for (uint32_t ix = 0; ix < poly_store.size; ++ix) {
        dest[ix].points = polys[ix].points;
        dest[ix].count = polys[ix].store.size;
    }
    *out_size = poly_store.size;
    LIST_FREE(polys, poly_store);

    return dest;
}
