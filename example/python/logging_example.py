from galaxy_py import glogging


def main():
    logger = glogging.get_logger('test', "/galaxy/aa-d/ttl=1d")

    logger.info("this is an info test")
    logger.warning("This is a warning test")
    logger.error("This is an error test")
    logger.debug("This is a debug test")
    logger.critical("This is a critical test")
    logger.fatal("This is a fatal test")
    print(glogging.get_logger_file(logger))


if __name__ == "__main__":
    main()
