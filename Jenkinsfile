pipeline {
    agent none

    environment {
        // Environment variables needed for the github-release tool
        GITHUB_TOKEN = credentials('github-boden-release-token')
        GITHUB_USER = 'AshampooSystems'
        GITHUB_REPO = 'boden'
    }

    stages {
        stage('Check formatting') {
            environment {
               BAUER_PLATFORM = 'linux'
               BAUER_BUILD_SYSTEM = 'make'
               BAUER_CONFIG = 'Release'
               BAUER_PACKAGE_FOLDER = 'package'
               BAUER_PACKAGE_GENERATOR = 'TGZ'
            }
            agent {
               dockerfile {
                   filename 'Dockerfile_linux'
                   additionalBuildArgs '-t boden_linux'
                   label 'boden'
               }
            }
            stages {
                stage('Run clang-format') {
                    steps {
                        sh 'python build.py build --module FormatSources'
                    }
                }
                stage('Check for changes') {
                    steps {
                        script {
                            List<String> sourceChanged = sh(returnStdout: true, script: "git diff --name-only").split()
                            if(sourceChanged.size() > 0) {
                                String changedFiles = "Some files were changed by clang-format, make sure to format before committing:\n";
                                for (int i = 0; i < sourceChanged.size(); i++) {
                                    changedFiles += sourceChanged[i] + "\n";
                                    if(i > 10) {
                                        changedFiles += "...\n";
                                        break;
                                    }
                                }
                                println changedFiles
                                error(changedFiles)
                            }
                        }
                    }
                }
            }
        }
        stage('Documentation') {
            agent {
                dockerfile {
                    filename 'Dockerfile_documentation'
                    additionalBuildArgs '-t boden_documentation'
                    args '--volume ${WORKSPACE}:/boden'
                    label 'boden'
                }
            }
            steps {
                sh 'cd /boden && mkdir -p build/documentation'
                sh 'cd /boden/build/documentation && cmake ../../ -DCMAKE_INSTALL_PREFIX=/boden/boden-documentation'
                sh 'cd /boden/build/documentation && make boden_documentation'
                sh 'cd /boden/build/documentation && cmake -DCOMPONENT=documentation -P cmake_install.cmake'
                sh 'cd /boden/ && tar -zcvf boden-documentation.tar.gz boden-documentation'

                stash includes: 'build/**/*', name: 'boden_documentation_builddir'
                archiveArtifacts artifacts: 'boden-*.tar.gz', fingerprint: true

                sh 'cd /boden/build && mkdir package'
                sh 'cd /boden && cp boden-documentation.tar.gz build/package/'
                stash includes: 'build/package/*', name: 'documentation-packages'
            }
        }
        stage('Platforms') {
            parallel {
                stage('Linux') {
                    environment {
                        BAUER_PLATFORM = 'linux'
                        BAUER_BUILD_SYSTEM = 'make'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        dockerfile {
                            filename 'Dockerfile_linux'
                            additionalBuildArgs '-t boden_linux'
                            label 'boden'
                        }
                    }
                    stages {
                        stage('Build') {
                            steps {
                                unstash 'boden_documentation_builddir'
                                sh 'python build.py copy -f build/documentation/documentation'
                                sh 'python build.py build'
                            }
                        }

                        stage('Package') {
                            steps {
                                sh 'python build.py package'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'linux-packages'
                            }
                        }
                        stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'python build.py run --module testboden -- --out testresults/linux_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/linux_testboden.xml"

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py run --module testbodenui -- --out testresults/linux_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/linux_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
                stage('Android') {
                    environment {
                        BAUER_PLATFORM = 'android'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        dockerfile {
                            filename 'Dockerfile_android'
                            additionalBuildArgs  '-t boden_android'
                            label 'boden'
                        }
                    }
                    stages {
                        stage('Build') {
                            steps {
                                unstash 'boden_documentation_builddir'
                                sh 'python build.py prepare -b make -a arm64-v8a'
                                sh 'python build.py prepare -b make -a x86_64'

                                // This is only necessary for testing, move out once tests work again
                                sh 'python build.py prepare -a x86_64 -b AndroidStudio'

                                sh 'python build.py build -p android $BUILD_EXTRA_ARGS'
                            }
                        }
                        stage('Package') {
                            steps {
                                sh 'python build.py package -p android $BUILD_EXTRA_ARGS'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'android-packages'
                            }
                        }
                    }
                }
                stage('MacOS') {
                    environment {
                        BAUER_PLATFORM = 'mac'
                        BAUER_BUILD_SYSTEM = 'make'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent { label 'macOS' }
                    stages {
                        stage('Build') {
                            steps {
                                unstash 'boden_documentation_builddir'
                                sh 'python build.py copy -f build/documentation/documentation'
                                sh 'python build.py build'
                            }
                        }
                        stage('Package') {
                            steps {
                                sh 'python build.py package'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'macos-packages'
                            }
                        }
                        stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'python build.py run --module testboden --run-output-file testresults/mac_testboden.xml -- --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/mac_testboden.xml"

                                sh 'python build.py run --module testbodenui --run-output-file testresults/mac_testbodenui.xml -- --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/mac_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
                stage('IOS') {
                    environment {
                        BAUER_PLATFORM = 'ios'
                        BAUER_BUILD_SYSTEM = 'make'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                        BAUER_IDENTITY = 'iPhone Developer'
                    }
                    agent { label 'iOS' }
                    stages {
                        stage('Build') {
                            steps {
                                unstash 'boden_documentation_builddir'
                                sh 'python build.py copy -f build/documentation/documentation'
                                sh 'python build.py build'
                            }
                        }

                        stage('Package') {
                            steps {
                                sh 'python build.py package'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'ios-packages'
                            }
                        }

                        stage('Sign') {
                            environment {
                                JENKINS_USER = credentials('mac-jenkins-user')
                                BAUER_KEYCHAIN = credentials('jenkins-dev-certificate-keychain')
                            }
                            steps {
                                sh 'python build.py codesign --password $JENKINS_USER_PSW'
                            }
                        }

                        stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'python build.py run --module testboden --run-output-file testresults/ios_testboden.xml -- --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                                junit "testresults/ios_testboden.xml"

                                sh 'python build.py run --module testbodenui --run-output-file testresults/ios_testbodenui.xml -- --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                                junit "testresults/ios_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
            }
        }
        stage('Release') {
            when {
                branch "release/*"
            }
            agent {
                dockerfile {
                    filename 'Dockerfile_github'
                    args '--volume ${WORKSPACE}:/boden'
                }
            }

            steps {
                unstash 'android-packages'
                unstash 'ios-packages'
                unstash 'macos-packages'
                unstash 'linux-packages'
                unstash 'documentation-packages'

                script {
                    env.RELEASE_NAME = env.BRANCH_NAME.split('/')[1]
                }

                sh 'github-release delete --tag $RELEASE_NAME || true'
                sh 'github-release release --target $GIT_COMMIT --tag $RELEASE_NAME --name "boden - $RELEASE_NAME"'
                sh 'ls /boden/build/package'
                sh 'cd build/package && find . -exec sh -c \'github-release upload --tag $RELEASE_NAME --name $(basename $0) --file $0\' {} ";"'
            }
        }
    }
}