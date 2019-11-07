import os

def main():
    test_cases = ['A_%s' % i for i in range(1,6)] + ['B_%s' % i for i in range(1,4)] + ['C_%s' % i for i in range(1,3)] + ['D_1'] + ['E1_%s' % i for i in range(1,4)]
    for case in test_cases:
        os.system('./Code/parser ./Test_normal/%s.cmm > ./ir_code/%s.ir' % (case, case))

if __name__ == '__main__':
    main()