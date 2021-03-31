import os
import re
import sys
import click
import chardet
import logging
import datetime


def init_logger():
    log_format = "[%(filename)s %(lineno)d %(levelname)s] %(message)s "
    date_format = '%Y-%m-%d  %H:%M:%S %a '
    logging.basicConfig(level=logging.INFO,
                        format=log_format,
                        datefmt=date_format,
                        )

class CheckOut:
    def __init__(self, rtt_repo, rtt_branch):
        self.root = os.getcwd()
        self.rtt_repo = rtt_repo
        self.rtt_branch = rtt_branch

    def get_new_file(self):
        file_list = list()
        try:
            os.system('git remote add rtt_repo {}'.format(self.rtt_repo))
            os.system('git fetch rtt_repo')
            os.system('git reset rtt_repo/{} --soft'.format(self.rtt_branch))
            os.system('git status > git.txt')
        except Exception as e:
            logging.error(e)
            return None
        try:
            with open('git.txt', 'r') as f:
                file_lines = f.readlines()
        except Exception as e:
            logging.error(e)
            return None
        file_path = ''
        for line in file_lines:
            if 'new file' in line:
                file_path = line.split('new file:')[1].strip()
                logging.info('new file -> {}'.format(file_path))
            elif 'deleted' in line:
                file_path = line.split('deleted:')[1].strip()
                logging.info('deleted file -> {}'.format(file_path))
            elif 'modified' in line:
                file_path = line.split('modified:')[1].strip()
                logging.info('modified file -> {}'.format(file_path))
            else:
                continue

            file_list.append(file_path)

        return file_list

class FormatCheck:
    def __init__(self, file_list):
        self.file_list = file_list

    def __check_file(self, file_lines):
        line_num = 1
        check_result = False
        for line in file_lines:
            # check line start
            line_start = line.replace(' ', '')
            # find tab
            if line_start.startswith('\t'):
                logging.error("line[{}]: please use space replace tab at the start of this line.".format(line_num))
                check_result = False
            # check line end
            lin_end = line.split('\n')[0]
            if lin_end.endswith(' ') or lin_end.endswith('\t'):
                logging.error("line[{}]: please delete extra space at the end of this line.".format(line_num))
                check_result = False
            line_num += 1

        return check_result

    def check(self):
        logging.info("Start to check files format.")
        if len(self.file_list) == 0:
            logging.warning("There are no files to check license.")
            return 0
        encoding_check_result = True
        format_check_result = True
        for file_path in self.file_list:
            file_lines = ''
            code = ''
            if file_path.endswith(".c") or file_path.endswith(".h"):
                try:
                    with open(file_path, 'r') as f:
                        file = f.read()
                        file_lines = f.readlines()
                        # get file encoding
                        code = chardet.detect(file)['encoding']
                except Exception as e:
                    logging.error(e)
            else:
                continue

            if code != 'utf-8':
                logging.error("file: {} encoding not utf-8, please format it.".format(file_path))
                encoding_check_result = False

            format_check_result = self.__check_file(file_lines)    

        if not encoding_check_result or not format_check_result:
            logging.error("files format check fail.")
            return 1

        logging.info("files format check success.")

        return 0


class LicenseCheck:
    def __init__(self, file_list):
        self.file_list = file_list

    def check(self):
        current_year = datetime.date.today().year
        logging.info("current year: {}".format(current_year))
        if len(self.file_list) == 0:
            logging.warning("There are no files to check license.")
            return 0
        logging.info("Start to check files license.")
        check_result = True
        for file_path in self.file_list:
            if file_path.endswith(".c") or file_path.endswith(".h"):
                try:
                    with open(file_path, 'r') as f:
                        file = f.readlines()
                except Exception as e:
                    logging.error(e)
            else:
                continue

            if 'Copyright' in file[1] and 'SPDX-License-Identifier: Apache-2.0' in file[3]:
                license_year = re.search(r'2006-\d{4}', file[1]).group()
                true_year = '2006-{}'.format(current_year)
                if license_year != true_year:
                    logging.warning("file: {} license year: {} is not true: {}, please update.".format(file_path,
                                                                                                       license_year,
                                                                                                       true_year))
                else:
                    logging.info("file: {} license check success.".format(file_path))
            else:
                logging.error("file: {} license check fail.".format(file_path))
                check_result = False

        if not check_result:
            return 1

        return 0


@click.group()
@click.pass_context
def cli(ctx):
    pass

@cli.command()
@click.option(
    '--license',
    "license",
    required=False,
    type=click.BOOL,
    flag_value=True,
    help="Enable File license check.",
)
@click.argument(
    'repo', nargs=1, type=click.STRING, default='https://github.com/RT-Thread/rt-thread'
)
@click.argument(
    'branch', nargs=1, type=click.STRING, default='master'
)
def check(license, repo, branch):
    """
    check files license and format.
    """
    init_logger()
    # get modified files list
    checkout = CheckOut(repo, branch)
    file_list = checkout.get_new_file()
    if file_list is None:
        logging.error("chout files fail")
        sys.exit(1)

    # check modified files format
    format_check = FormatCheck(file_list)
    format_check_result = format_check.check()
    license_check_result = True
    if license:
        license_check = LicenseCheck(file_list)
        license_check_result = license_check.check()

    if format_check_result or license_check_result:
        logging.error("file format check or license check fail.")
        sys.exit(1)
    logging.info("check success.")
    sys.exit(0)


if __name__ == '__main__':
    cli()
