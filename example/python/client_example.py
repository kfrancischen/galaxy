from python import gclient

def main():
    gclient.create_dir_if_not_exist("/aa-d/Download/test_from_python")
    print(gclient.list_dirs_in_dir("/aa-d/Download"))

if __name__ == "__main__":
    main()
