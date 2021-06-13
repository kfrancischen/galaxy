import logging
from galaxy_py import GalaxyLoggingHandler


def main():
    logger = logging.getLogger("test")
    logger.setLevel(logging.INFO)
    handler = GalaxyLoggingHandler("test", "/galaxy/aa-d/ttl=1d")
    logger.addHandler(handler)

    logger.info("this is an info test")
    logger.warn("This is a warning test")
    logger.error("This is an error test")
    logger.debug("This is a debug test")
    logger.critical("This is a critical test")
    logger.fatal("This is a fatal test")


if __name__ == "__main__":
    main()
