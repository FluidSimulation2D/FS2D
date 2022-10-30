import requests
import zipfile
import os
import re

if __name__ == "__main__":
    print("\nDownload dependencies")

    libs = {"SFML-2.5.1" : "https://www.sfml-dev.org/files/SFML-2.5.1-windows-vc15-64-bit.zip",
            "TGUI-1.0" : "https://github.com/texus/TGUI/releases/download/v1.0-alpha/TGUI-1.0-alpha-VisualStudio-64bit-for-SFML-2.5.1.zip" }
    externalPath = "../external"

    try:
        if not os.path.exists(externalPath):
            os.mkdir(externalPath)
        os.chdir(externalPath)

        for name, url in libs.items():

            print("-> Downloading {} from: {}".format(name, url))

            if os.path.exists("{}/{}".format(externalPath, name)):
                print("-> {} already downloaded".format(name))
                continue

            with open("{}.zip".format(name), "wb") as file:
                file.write(requests.get(url).content)

            print("-> Extracting files from archive")
            with zipfile.ZipFile("{}.zip".format(name)) as zip:
                zip.extractall("./")

            os.remove("{}.zip".format(name))

            print("-> Success {}".format(name))

        print("Successful download dependencies")

    except BaseException as e:
        print("-> Error was occured: {}\nFail".format(e))
        exit(0)
