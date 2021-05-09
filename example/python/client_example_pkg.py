from galaxy_py import gclient

def main():
    gclient.create_dir_if_not_exist("/galaxy/aa-d/Download/test_from_python")
    print(gclient.list_dirs_in_dir("/galaxy/aa-d/Download"))
    print(gclient.llist_dirs_in_dir("/home/pslx/Download"))

if __name__ == "__main__":
    main()
