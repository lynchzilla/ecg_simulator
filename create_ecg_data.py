#-------------------------------------------------------------------------------
# Name:        create_ecg_data
# Purpose:     create ecg waveform data in C language format
#
# Author:      Jim Lynch
# Language:    Python
# Created:     22/07/2012
# Copyright:   (c) Jim 2012
# Licence:     <Open Source>
#
# Note: tested with Python 3.2.3  (latest and greatest in July 2012)
#-------------------------------------------------------------------------------
#!/usr/bin/env python

import operator

def main():
    pass

if __name__ == '__main__':
    main()

    ################################################################################
    #            interpolate(x, x_values, y_values)
    #
    #  Description:  return y at point x using linear interpolation
    #
    #  Input: x = input x-value
    #         x_values = table of digitized points along horizontal x-axis (time)
    #         y_values = table of digitized points along vertical y-axis (amplitude)
    #
    #
    #  Returns:  y = y-value at the point x
    #
    #
    #  Reference: Dr. Dogan Ibrahim
    #             "Interpolation in microcomputer based instrumentation and
    #              Control Projects"
    ################################################################################
    def interpolate(x, x_values, y_values):

        # find the table points on either side of the input value x
        # note: a is the x-index before and b is the x-index after
        i = 0
        while (x > x_values[i]):
            i = i + 1

        # xa = x value before the input x
        # xb = x value after the input x
        xa = x_values[i - 1]
        xb = x_values[i]

        # fa = y value at xa
        # fb = y value at xb
        fa = y_values[i - 1]
        fb = y_values[i]

        # the linear interpolation formula
        y1 = fa * (xb - x) / (xb - xa)
        y2 = fb * (x - xa) / (xb - xa)
        y = y1 + y2

        # return the y value at point x
        return y


    #---------------------------------------------------------------------------
    #                   WAVEFORM GENERATOR UTILITY
    #
    #
    #  Purpose: Create a table of ECG waveform points, spaced every 1.0 msec
    #
    #
    #  Input: Digitized waveform from Engauge (open source, digitizing software)
    #         Engauge may be download from SourceForge
    #
    #
    #         File: SampledECGWaveform.txt has x,y values (1 point per line)
    #
    #               x,Curve1            <====== THIS LINE WILL BE REMOVED
    #               0.217621,0.117082
    #               0.225077,0.124456
    #               0.23555,0.169024
    #               0.241553,0.213633
    #               0.246067,0.258256
    #               0.253545,0.287963
    #               0.261038,0.332559
    #               0.268524,0.36971
    #                   :        :
    #               0.746517,0.149381
    #               0.75547,0.164186
    #               0.761444,0.179019
    #
    #  Processing:  Data points are read from above file and using linear
    #               interpolation are converted into samples spaced every 1 msec
    #
    #               y-axis data is scaled to 0 .. 4096
    #               (for Microchip MCP4921-E/P-ND 12-bit D/A converter)
    #
    #
    #  Output:  text file contains single table of y-values spaced every 1 msec
    #           file is formatted to look like a C array with initialization.
    #
    #       const short  y_data[] = {
    #       936, 938, 939, 940, 941, 943, 944, 945, 949, 954,
    #       959, 965, 970, 976, 981, 986, 992, 997, 1003, 1012,
    #       1022, 1031, 1041, 1050, 1060, 1072, 1085, 1097, 1110, 1118,
    #       1123, 1128, 1133, 1138, 1143, 1148, 1154, 1161, 1169, 1176,
    #       1184, 1191, 1199, 1206, 1213, 1219, 1226, 1232, 1238, 1245,
    #       1251, 1257, 1261, 1265, 1268, 1272, 1276, 1280, 1283, 1283,
    #       1281, 1278, 1276, 1273, 1270, 1268, 1265, 1263, 1260, 1258,
    #       1255, 1253, 1250, 1248, 1243, 1238, 1234, 1229, 1224, 1219,
    #       1215, 1212, 1208, 1204, 1200, 1196, 1192, 1188, 1181, 1175,
    #       1168, 1162, 1156, 1149, 1143, 1138, 1133, 1128, 1123, 1117,
    #       1112, 1107, 1101, 1093, 1085, 1078, 1070, 1062, 1055, 1047,
    #       1037, 1028, 1018, 1010, 1001, 993, 984, 979, 975, 972,
    #       968, 964, 960, 956, 953, 950, 946, 943, 940, 937,
    #       935, 937, 938, 939, 940, 942, 943, 944, 944, 944,
    #       944, 944, 944, 944, 944, 945, 947, 949, 951, 954,
    #       956, 958, 960, 962, 963, 963, 963, 963, 963, 963,
    #       961, 958, 955, 951, 948, 945, 942, 939, 935, 930,
    #       924, 917, 911, 905, 898, 892, 882, 863, 818, 731,
    #       604, 553, 506, 630, 696, 750, 805, 894, 975, 1021,
    #       1066, 1124, 1234, 1344, 1454, 2080, 2241, 2468, 2543, 2588,
    #       2634, 2689, 3076, 3127, 3179, 3209, 3307, 3395, 3484, 3572,
    #       3795, 3838, 3881, 3789, 3434, 3444, 3289, 3045, 2812, 2803,
    #       2220, 2252, 1888, 1730, 1621, 995, 901, 354, 375, 203,
    #       30, 32, 61, 90, 119, 160, 237, 275, 291, 308,
    #       325, 343, 370, 398, 428, 483, 541, 601, 650, 701,
    #       756, 800, 836, 854, 873, 893, 914, 936, 965, 1014,
    #       1032, 1038, 1045, 1051, 1057, 1064, 1064, 1061, 1058, 1056,
    #       1053, 1051, 1048, 1046, 1043, 1041, 1038, 1035, 1033, 1030,
    #       1028, 1025, 1022, 1020, 1017, 1014, 1011, 1009, 1006, 1003,
    #       1001, 998, 997, 995, 993, 992, 990, 989, 987, 986,
    #       984, 982, 981, 979, 976, 974, 971, 968, 966, 963,
    #       961, 961, 961, 961, 961, 961, 961, 961, 961, 961,
    #       961, 961, 961, 961, 961, 961, 961, 961, 961, 961,
    #       962, 963, 964, 965, 966, 967, 968, 969, 970, 972,
    #       974, 976, 978, 980, 982, 984, 986, 989, 991, 993,
    #       995, 997, 1000, 1004, 1008, 1012, 1017, 1021, 1025, 1029,
    #       1033, 1038, 1043, 1047, 1052, 1057, 1061, 1067, 1073, 1079,
    #       1086, 1092, 1098, 1105, 1111, 1117, 1123, 1130, 1138, 1146,
    #       1155, 1163, 1170, 1175, 1180, 1185, 1190, 1195, 1200, 1205,
    #       1211, 1218, 1224, 1230, 1237, 1243, 1247, 1251, 1256, 1260,
    #       1266, 1274, 1283, 1291, 1300, 1306, 1312, 1318, 1325, 1331,
    #       1337, 1339, 1341, 1344, 1346, 1348, 1350, 1352, 1354, 1356,
    #       1356, 1356, 1356, 1356, 1355, 1353, 1351, 1348, 1346, 1344,
    #       1342, 1340, 1338, 1335, 1333, 1331, 1328, 1326, 1324, 1321,
    #       1319, 1316, 1314, 1312, 1309, 1304, 1297, 1291, 1285, 1278,
    #       1272, 1267, 1262, 1257, 1252, 1248, 1243, 1237, 1230, 1224,
    #       1218, 1211, 1205, 1198, 1191, 1183, 1175, 1167, 1159, 1151,
    #       1145, 1141, 1137, 1133, 1128, 1124, 1120, 1116, 1111, 1104,
    #       1096, 1087, 1079, 1071, 1066, 1061, 1056, 1051, 1046, 1041,
    #       1035, 1031, 1027, 1023, 1018, 1014, 1010, 1007, 1003, 999,
    #       995, 991, 987, 989, 990, 992, 993, 995, 996, 995,
    #       994, 993, 992, 991, 990, 989, 988, 987, 987, 987,
    #       987, 987, 987, 987, 985, 984, 982, 980, 979, 978,
    #       980, 982, 984, 986, 988, 990, 992, 994, 997, 1000,
    #       1003, 1006, 1009};
    #
    #           This text file can be easily cut-and-pasted into an Arduino
    #           sketch.
    #
    #---------------------------------------------------------------------------

    # create some empty lists
    ecg_pt_x = []
    ecg_pt_y = []
    ecg_x = []
    ecg_y = []
    ecgx = []
    ecgy = []
    ecg_xdata_ms = []
    ecg_ydata_dtoa = []

    # open the waveform text file (from engauge)
    # note: waveform file should be in the same folder as the python program (c:\projects\)
    # file = open("SampledECGWaveform.txt")
    file = open("doodah.txt")

    # read and discard the first line of the file  ( x,Curve1 )
    line = file.readline()

    # endless loop to read and process the waveform file
    while 1:

        # read a line from the raw waveform file
        # note: each line in the file is a string: '0.297647,0.321337\n'
        line = file.readline()

        # break (exit from endless loop) when there are no more lines to read
        if not line:
            break

        # process the file
        pass

        # remove the '\n' newline character
        # line = '0.297647,0.321337'
        line = line.rstrip('\n')

        # split the string into a list at the comma separator
        # linelist = ['0.297647', '0.321337']
        linelist = line.split(',')

        # now extract the x-value and y-value and convert to float
        x = float(linelist[0])
        y = float(linelist[1])

        # add these x and y values to the respective point lists
        # ecg_pt_x = [0.297647, 4.45898, 9.21484] ... and so on
        # ecg_pt_y = [0.321337, 0.0, -0.321337]   ... and so on
        ecg_pt_x.append(x)
        ecg_pt_y.append(y)

    # note the size of each list
    lengthx = len(ecg_pt_x)
    lengthy = len(ecg_pt_y)

    # note the end points of the lists
    xstart = ecg_pt_x[0]
    ystart = ecg_pt_y[0]
    xend = ecg_pt_x[lengthx - 1]
    yend = ecg_pt_y[lengthy - 1]

    # x-axis is horizontal (time)
    # total time from start to finish is about 434 milliseconds

    # x_axis: adjust x-axis so it starts at zero
    for i in range(lengthx):
        if (xstart < 0):
            ecg_pt_x[i] = ecg_pt_x[i] + xstart
        else:
            ecg_pt_x[i] = ecg_pt_x[i] - xstart

    # recalculate the start/end points
    xstart = ecg_pt_x[0]
    xend = ecg_pt_x[lengthx - 1]
    xspan = xend - xstart

    # y-axis is vertical  (amplitude)
    # total span from min to max should be 0.0 .. 4096.0
    # (Microchip MCP4921-E/P-ND 12-bit D/A - unipolar)

    # calculate the y-axis max and min values
    ymin = min(ecg_pt_y)
    ymax = max(ecg_pt_y)

    # adjust y-axis so it's minimum is zero
    for i in range(lengthy):
        if (ymin < 0):
            ecg_pt_y[i] = ecg_pt_y[i] - ymin
        else:
            ecg_pt_y[i] = ecg_pt_y[i] + ymin

    # calculate the y-axis span
    yspan = ymax - ymin
    yscalefactor = 4096 / yspan

    # scale y-axis to 0 .. 4096
    for i in range(lengthy):
        ecg_pt_y[i] = ecg_pt_y[i] * yscalefactor

    # convert ecg_pt_x and ecg_pt_y lists to a list of tuples
    ecg_data = list(zip(ecg_pt_x, ecg_pt_y))

    # sort list of tuples by ecg_pt_x (Time axis)
    ecg_data.sort(key=operator.itemgetter(0))

    # extract list of x-values and list of y-values (both same size)
    for i in range(len(ecg_data)):
        ecg_x.append(ecg_data[i][0])
        ecg_y.append(ecg_data[i][1])

    # convert x-values into milliseconds
    for i in range(len(ecg_data)):
        ecg_x[i] = ecg_x[i] * 1000.0

    # create ecg_xdata[] and ecg_ydata[] tables at 1 msec intervals
    x = 0.0
    for i in range(int(ecg_x[-1])):

        # linear interpolate every 1.0 msec
        y = interpolate(x, ecg_x, ecg_y)

        # add to ecg_xdata_ms[] and ecg_ydata_dtoa[] tables
        ecg_xdata_ms.append(int(x))
        ecg_ydata_dtoa.append(int(y))
        x = x + 1.000

    # create a file to write the ecg_ydata to
    FILE = open("ecg_ydata_dtoa.txt","w")

    # create a C language array definition of the y-values, 10 values per line
    j = 0
    string = "const short  y_data[] = {\n"
    FILE.write(string)
    string = ''

    for i in range(int(ecg_x[-1])):

        string = string + str(ecg_ydata_dtoa[i]) + ', '
        j = j + 1

        # reached the last element?
        if (i == int(ecg_x[-1]) - 1):

            # reached the last element, must close line with a brace
            # convert string into a list
            liststring = list(string)

            # remove trailing space and comma
            del liststring[-1]
            del liststring[-1]

            # convert back to string
            string = ''.join(liststring)

            # write final string to file with closing brace and semicolon
            FILE.write(string + '};\n')

        else:

            # write next line to file
            if (j >= 10):
                FILE.write(string + '\n')
                string = ''
                j = 0

    # all done, close file "ecg_ydata_dtoa.txt"
    # (it may be used in Arduino waveform generator program)
    FILE.close()

