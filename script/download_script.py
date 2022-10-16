import requests
import zipfile
import os

if __name__ == "__main__":
    print("\nDownload dependencies")

    if os.path.exists("../external/SFML"):
        print("-> SFML already downloaded\nSuccess")
        exit(0)

    try:
        externalPath = "../external"
        os.mkdir(externalPath)
        os.chdir(externalPath)

        URL = "https://www.sfml-dev.org/files/SFML-2.5.1-windows-vc15-64-bit.zip"
        print("-> Downloading sfml library from: " + URL)
        with open("sfml.zip", "wb") as file:
            file.write(requests.get(URL).content)

        print("-> Extracting files from archive")
        with zipfile.ZipFile("sfml.zip") as zip:
            zip.extractall("./")

        os.rename("SFML-2.5.1", "SFML")
        os.remove("sfml.zip")

        print("Success")
    except BaseException:
        print("-> Error was occured\nFail")
        exit(0)