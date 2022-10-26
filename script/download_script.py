import requests
import zipfile
import os

if __name__ == "__main__":
    print("\nDownload dependencies")

    libs_names = {"SFML" : "SFML-2.5.1",
                  "TGUI" : "TGUI-1.0" }
    
    libs = {"SFML" : "https://www.sfml-dev.org/files/SFML-2.5.1-windows-vc15-64-bit.zip",
            "TGUI" : "https://github.com/texus/TGUI/releases/download/v1.0-alpha/TGUI-1.0-alpha-VisualStudio-64bit-for-SFML-2.5.1.zip" }

    try:
        externalPath = "../external"
        os.mkdir(externalPath)
        os.chdir(externalPath)

        for name, url in libs.items():

            if os.path.exists("../external/{}".format(name)):
                print("-> {} already downloaded\n".format(name))
                continue
            
            print("-> Downloading {} library from: {}".format(name).format(url))
            with open("{}.zip".format(name), "wb") as file:
                file.write(requests.get(url).content)

            print("-> Extracting files from archive")
            with zipfile.ZipFile("{}.zip".format(name)) as zip:
                zip.extractall("./")

            os.rename(libs_names[name], name)
            os.remove("{}.zip".format(name))

            print("Success with {}".format(name))
            
    except BaseException:
        print("-> Error was occured\nFail")
        exit(0)
