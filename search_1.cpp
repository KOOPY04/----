#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream>

using namespace std;

struct Record {
    string student_id;  // �ǥ�ID
    string course_id;   // �ҵ{ID
    string course_name; // �ҵ{�W��
};

// Ū���ǥͯ��ޤ��
unordered_map<string, long> readStudentIndex(const string& filename) {
    unordered_map<string, long> index;
    ifstream infile(filename);
    if (!infile) {
        cerr << "�L�k���}�ǥͯ��ޤ��: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    string studentId;
    long offset;
    while (infile >> studentId >> offset) {
        index[studentId] = offset;
    }

    infile.close();
    return index;
}

// Ū���ҵ{���ޤ��
unordered_map<string, long> readCourseIndex(const string& filename) {
    unordered_map<string, long> index;
    ifstream infile(filename);
    if (!infile) {
        cerr << "�L�k���}�ҵ{���ޤ��: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    string courseId;
    long offset;
    while (infile >> courseId >> offset) {
        index[courseId] = offset;
    }

    infile.close();
    return index;
}

// �ھھǥ�ID�d��
void searchByStudentID(const unordered_map<string, long>& studentIndex, const vector<string>& dataFiles, const string& studentID) {
    auto it = studentIndex.find(studentID);
    if (it == studentIndex.end()) {
        cout << "�䤣��ǥ�ID " << studentID << " ������ҵ{�C" << endl;
        return;
    }

    long offset = it->second;
    int countCourse = 0;
    ofstream outFile("student_query_result.txt");
    if (!outFile.is_open()) {
        cerr << "�L�k���}���G���: student_query_result.txt" << endl;
        return;
    }

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "�L�k���}�ƾڤ��: " << dataFile << endl;
            continue;
        }

        string line;
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == studentID) {
                outFile << "�ҵ{ID: " << record.course_id << ", �ҵ{�W��: " << record.course_name << '\n';
                countCourse++;
            }

            offset += line.length() + 1; // ��s�����q�]�]�t����š^
        }

        dataFp.close();
    }
    
    outFile << "�ǥ�ID " << studentID << " �@��Ū�F " << countCourse << " ���ҵ{" << endl;
    outFile.close();
    cout << "�d�ߵ��G�w�g�J student_query_result.txt" << endl;
}


// �ھڽҵ{ID�d��
void searchByCourseID(const unordered_map<string, long>& courseIndex, const vector<string>& dataFiles, const string& courseID) {
    auto it = courseIndex.find(courseID);
    if (it == courseIndex.end()) {
        cout << "�䤣��ҵ{ID " << courseID << " ������ǥ͡C" << endl;
        return;
    }

    long offset = it->second;
    int countStudent = 0;
    string courseName; // �Ω�s�x�ҵ{�W��
    bool foundCourseName = false; // �аO�O�_�w���ҵ{�W��

    ofstream outFile("course_query_result.txt");
    if (!outFile.is_open()) {
        cerr << "�L�k���}���G���: course_query_result.txt" << endl;
        return;
    }

    // �M���Ҧ��ƾڤ��
    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "�L�k���}�ƾڤ��: " << dataFile << endl;
            continue;
        }

        string line;
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.course_id == courseID) {
                if (!foundCourseName) {
                    courseName = record.course_name; // ����ҵ{�W��
                    foundCourseName = true;
                    // �b���}�Y�g�J�ҵ{ID�M�ҵ{�W��
                    outFile << "�ҵ{ID: " << courseID << ", �ҵ{�W��: " << courseName << '\n';
                }
                outFile << "�ǥ�ID: " << record.student_id << '\n';
                countStudent++;
            }

            offset += line.length() + 1; // ��s�����q�]�]�t����š^
        }

        dataFp.close();
    }

    if (!foundCourseName) {
        cout << "�䤣��ҵ{ID " << courseID << " ���ҵ{�W�١C" << endl;
    } else {
        outFile << "�ҵ{ID " << courseID << " �@�� " << countStudent << " ��ǥ�" << endl;
    }

    outFile.close();
    cout << "�d�ߵ��G�w�g�J course_query_result.txt" << endl;
}


// �c�y�ƾڤ��W�٦C��
vector<string> generateDataFileNames(int fileCount) {
    vector<string> dataFiles;
    for (int i = 1; i <= fileCount; i++) {
        stringstream fileNo;
        if (i < 10) {
            fileNo << "data_big5/000" << i;
        } else if (i < 100) {
            fileNo << "data_big5/00" << i;
        } else {
            fileNo << "data_big5/0" << i;
        }
        dataFiles.push_back(fileNo.str());
    }
    return dataFiles;
}

int main() {
    const string studentIndexFile = "student_index.txt"; // �ǥ�ID���ޤ��W
    const string courseIndexFile = "course_index.txt";   // �ҵ{ID���ޤ��W

    // Ū���ǥͯ��ޤ��
    unordered_map<string, long> studentIndex = readStudentIndex(studentIndexFile);

    // Ū���ҵ{���ޤ��
    unordered_map<string, long> courseIndex = readCourseIndex(courseIndexFile);

    // ���]�z���@�Ө�ƥi�H�ͦ����W�C��
    vector<string> dataFiles = generateDataFileNames(466); // ���]�o�̤w�g�ͦ��F�ƾڤ��W�C��

    char choice;
    cout << "�п�ܭn�d�ߪ����� ('I' ���ǥ�ID, 'C' ���ҵ{ID): ";
    cin >> choice;

    if (choice == 'I') {
        string studentID;
        cout << "�п�J�n�d�ߪ��ǥ�ID (�Ҧp: D1149488): ";
        cin >> studentID;

        searchByStudentID(studentIndex, dataFiles, studentID);
    } else if (choice == 'C') {
        string courseID;
        cout << "�п�J�n�d�ߪ��ҵ{ID (�Ҧp: 2142): ";
        cin >> courseID;

        searchByCourseID(courseIndex, dataFiles, courseID);
    } else {
        cout << "�L�Ī���ܡA�п�J 'I' �� 'C'\n";
    }

    return 0;
}

