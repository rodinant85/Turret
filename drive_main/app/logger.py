#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import logging
import sys
import os


os.makedirs('logs', exist_ok=True)


def info_log(name, file='logs/INFO_LOG.txt', encoding='utf-8'):
    log = logging.getLogger(name)
    log.setLevel(logging.INFO)

    formatter = logging.Formatter('[%(asctime)s] %(name)s %(levelname)-8s %(message)s')

    from logging.handlers import RotatingFileHandler
    fh = RotatingFileHandler(file, maxBytes=2000000, backupCount=5, encoding=encoding)
    fh.setFormatter(formatter)
    log.addHandler(fh)

    sh = logging.StreamHandler(stream=sys.stdout)
    sh.setFormatter(formatter)
    log.addHandler(sh)

    return log


def exc_log(name, file='logs/EXC_LOG.txt', encoding='utf-8'):
    log = logging.getLogger(name)
    log.setLevel(logging.ERROR)

    formatter = logging.Formatter('[%(asctime)s] %(name)s %(lineno)d %(levelname)-8s %(message)s')

    from logging.handlers import RotatingFileHandler
    fh = RotatingFileHandler(file, maxBytes=2000000, backupCount=5, encoding=encoding)
    fh.setFormatter(formatter)
    log.addHandler(fh)

    sh = logging.StreamHandler(stream=sys.stderr)
    sh.setFormatter(formatter)
    log.addHandler(sh)

    return log
