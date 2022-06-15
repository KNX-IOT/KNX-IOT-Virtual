#!/usr/bin/env python
#############################
#
#    copyright 2022 Cascoda
#    Redistribution and use in source and binary forms, with or without modification,
#    are permitted provided that the following conditions are met:
#    1.  Redistributions of source code must retain the above copyright notice,
#        this list of conditions and the following disclaimer.
#    2.  Redistributions in binary form must reproduce the above copyright notice,
#        this list of conditions and the following disclaimer in the documentation
#        and/or other materials provided with the distribution.
#
#    THIS SOFTWARE IS PROVIDED "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE OR
#    WARRANTIES OF NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL THE
#    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
#    OR CONSEQUENTIAL DAMAGES
#    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#    LOSS OF USE, DATA, OR PROFITS;OR BUSINESS INTERRUPTION)
#    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#    EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#############################
import os
import sys
import argparse
import time
import os.path
from os import path

if __name__ == '__main__':  # pragma: no cover

    parser = argparse.ArgumentParser()

    # input (files etc.)
    parser.add_argument("-app", "--application",
                    help="the application executable", nargs='?',
                    const=1, required=True)
    parser.add_argument("-sleep", "--sleep",
                    help="sleep before restarting", nargs='?',
                    const=1, required=False)
    # (args) supports batch scripts providing arguments
    print(sys.argv)
    args = parser.parse_args()
    
    print("--------restart_app---------")
    print("application      :" + str(args.application))
    print("sleep            :" + str(args.sleep))
    
    if path.isfile(str(args.application)) == False:
        print(" file {} not found".format(str(args.application)))
    else:
        while 1:
            print ("starting :", str(args.application))
            os.system(str(args.application))
            if args.sleep is not None:
                time.sleep(int(args.sleep))
            print("shutdown -> restart")
        exit()