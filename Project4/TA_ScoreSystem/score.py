#!/usr/bin/env python3

import os
import sys
from collections import namedtuple


DELIMITER = '|'
FlagPoints = namedtuple('FlagPoints', ['index', 'flag', 'points'])


def get(f):
    return list(map(
        lambda x: list(map(lambda x: x.strip(), x.strip().split(DELIMITER))),
        filter(lambda x: x.strip(), f.readlines()),
    ))


def check_input_filename(filename):
    filename = os.path.basename(filename)
    if not filename:
        raise ValueError('You did not provide filename')
    if len(filename.split('.')) != 1:
        raise ValueError('Your file include extension')


def check_student_ids(student_ids):
    for student in student_ids:
        if not student:
            raise ValueError('Empty student id')
        if len(student) < 3:
            raise ValueError('Student id too short (< 3)')


def check_key_is_unique(v):
    k = [i[0] for i in v]
    if len(k) != len(set(k)):
        raise KeyError('Duplicate key')


def main(input_fn: str, answer_fns: list):
    check_input_filename(input_fn)

    with open(input_fn) as f:
        student_ids = next(f).strip().split(',')
        input_lines = get(f)

    check_student_ids(student_ids)
    check_key_is_unique(input_lines)

    answers = []
    total_answers = 0
    for ans_fn in answer_fns:
        with open(ans_fn) as f:
            lines = get(f)
            total_answers = len(lines)
            answers.append({k: FlagPoints(
                i, v, int(s)) for i, (k, v, s) in enumerate(lines)})

    score = 0
    correct = ['0'] * total_answers
    for k, v in input_lines:
        for ans in answers:
            if ans[k].flag == v:
                score += ans[k].points
                correct[ans[k].index] = '1'

    for student in student_ids:
        print(f'{student}, {score}, {", ".join(correct)}')


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2:])
