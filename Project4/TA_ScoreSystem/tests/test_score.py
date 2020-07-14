# -*- coding: utf-8 -*-

import pytest

from score import main


def test_filename():
    with pytest.raises(ValueError):
        main('', [])

    with pytest.raises(ValueError):
        main('foo.txt', [])

    with pytest.raises(ValueError):
        main('foo.', [])


def test_student_id(fs):
    fs.create_file('/foo', contents='06')
    with pytest.raises(ValueError):
        main('/foo', [])


def test_key_is_unique(fs):
    contents = [
        '0650000',
        '1-fidles|FLAG_1',
        '1-fidles|FLAG_1',
        '1-fidles|FLAG_1'
    ]
    fs.create_file('/foo', contents='\n'.join(contents))
    with pytest.raises(KeyError):
        main('/foo', [])


def test_multiple_ansfile(fs, capsys):
    i1 = [
        '0650000',
        'foo|F1',
        'bar|F4'
    ]
    i2 = [
        '0650000',
        'foo|F3',
        'bar|F4'
    ]
    a1 = [
        'foo|F1|20',
        'bar|F2|30'
    ]
    a2 = [
        'foo|F3|20',
        'bar|F4|30'
    ]
    fs.create_file('/a1', contents='\n'.join(a1))
    fs.create_file('/a2', contents='\n'.join(a2))
    fs.create_file('/i1', contents='\n'.join(i1))
    fs.create_file('/i2', contents='\n'.join(i2))

    main('/i1', ['/a1', '/a2'])
    captured = capsys.readouterr()
    assert captured.out == '0650000, 50, 1, 1\n'

    main('/i2', ['/a1', '/a2'])
    captured = capsys.readouterr()
    assert captured.out == '0650000, 50, 1, 1\n'
