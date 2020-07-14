# -*- coding: utf-8 -*-

import asyncio
import glob
import pathlib
import sys
import tempfile
import zipfile

import aiohttp

import multiscore
from e3dump.auth import login
from e3dump.fetcher import download_file


DUMP_URL = 'https://e3new.nctu.edu.tw/mod/assign/view.php?id=45912&action=downloadall'


class Foo:
    def __init__(self, inputs, answer):
        self.inputs = inputs
        self.answer = answer

    def __call__(self):
        return self

    def parse_args(self):
        return Foo(self.inputs, self.answer)


async def main(username, password):
    async with aiohttp.ClientSession() as client:
        success, root = await login(client, username, password)
        if not success:
            print('Failed')
            return

        with tempfile.TemporaryDirectory() as temp_dir:
            zip_path = pathlib.Path('%s/out.zip' % (temp_dir))
            ext_dir = pathlib.Path('%s/goo' % (temp_dir))
            await download_file(client, DUMP_URL, zip_path)

            zip_ref = zipfile.ZipFile(zip_path, 'r')
            zip_ref.extractall(ext_dir)
            zip_ref.close()

            files = glob.glob('%s/*/*' % (ext_dir))
            ans = ['../ans1', '../ans2']
            multiscore.get_parser = Foo(files, ans)

            print(f'Total submission: {len(files)}')
            multiscore.main()


if __name__ == '__main__':
    username = sys.argv[1]
    password = sys.argv[2]

    loop = asyncio.get_event_loop()
    loop.run_until_complete(main(username, password))
