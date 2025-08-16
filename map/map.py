import subprocess


urlbase = "https://map-view.nls.uk/iiif/2/23127%2F231272292"


for x in range(0, 16):
    for y in range(0, 11):

        filename = f"tile_{x}_{y}.png"
        url = urlbase + f"/{x * 1024},{y * 1024},1024,1024/512,/0/gray.png"
        print(f"Getting {url}:")
        subprocess.run(["curl.exe", url, "--output", filename])

        print(f"Created tile_{x}_{y}.png")
