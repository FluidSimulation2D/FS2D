import requests
import zipfile
import os
import re

if __name__ == "__main__":
    print("\nDownload dependencies")
    
    libs = { "SFML" : "https://www.sfml-dev.org/files/SFML-2.5.1-windows-vc15-64-bit.zip",
             "TGUI" : "https://github.com/texus/TGUI/releases/download/v1.0-alpha/TGUI-1.0-alpha-VisualStudio-64bit-for-SFML-2.5.1.zip" }
    
    try:
        externalPath = "../external"
        if not os.path.exists(externalPath):
            os.mkdir(externalPath)
        os.chdir(externalPath)
        
        for name, url in libs.items():
            
            print("-> Downloading {} from: {}".format(name, url))
            
            if os.path.exists(externalPath + "/{}".format(name)):
                print("-> {} already downloaded".format(name))
                continue

            libRegex = r'^{}[\d\_\-\.]+$'.format(name)
            with open("{}.zip".format(name), "wb") as file:
                file.write(requests.get(url).content)

            print("-> Extracting files from archive")
            with zipfile.ZipFile("{}.zip".format(name)) as zip:
                zip.extractall("./")

            extractDir = [curDir for curDir in os.listdir() if re.fullmatch(libRegex, curDir)][0]
            os.rename(extractDir, name)
            os.remove("{}.zip".format(name))

            print("-> Success {}".format(name))

        print("Successful download dependencies")
            
    except BaseException as e:
        print("-> Error was occured: {}\nFail".format(e))
        exit(0)
