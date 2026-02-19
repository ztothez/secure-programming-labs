pipeline {
  agent any

  tools {
    jdk 'jdk21'
    nodejs 'node22'
  }

  environment {
    IMAGE = "ztothez/juice-shop:latest"
  }

  options {
    timestamps()
  }

  stages {

    stage('Clean Workspace') {
      steps {
        cleanWs()
      }
    }

    stage('Checkout from Git') {
      steps {
        git branch: 'master',
            url: 'https://github.com/musfiqur-m/juice-shop.git'
      }
    }

    // =========================
    // Task 2: SAST - Semgrep JSON (NO pip/sudo)
    // =========================
    stage('SAST - Semgrep (JSON)') {
      steps {
        sh '''
          set -eu
          echo "[+] Semgrep (Docker) -> semgrep-results.json"

          docker run --rm \
            -v "$PWD:/src" -w /src \
            returntocorp/semgrep:latest \
            semgrep --config auto --json --output semgrep-results.json .

          ls -la semgrep-results.json
        '''
      }
    }

    // =========================
    // Task 3: SBOM JSON (Syft via Docker)
    // =========================
    stage('Install Dependencies + SBOM (JSON)') {
      steps {
        sh '''
          set -eu
          node --version
          npm --version
          npm install

          echo "[+] SBOM (Syft Docker) -> sbom.cdx.json"
          docker run --rm \
            -v "$PWD:/src" -w /src \
            anchore/syft:latest \
            dir:. -o cyclonedx-json=sbom.cdx.json

          ls -la sbom.cdx.json
        '''
      }
    }

    // =========================
    // SCA - Snyk (produce JSON for Task 7 evidence)
    // =========================
    stage('SCA - Snyk (JSON Report)') {
      steps {
        script {
          def snykHome = tool name: 'snyk',
            type: 'io.snyk.jenkins.tools.SnykInstallation'

          withCredentials([string(credentialsId: 'snyk-token-id', variable: 'SNYK_TOKEN')]) {
            sh """
              set -eu

              if [ -x "${snykHome}/snyk-linux" ]; then
                SNYK_BIN="${snykHome}/snyk-linux"
              elif [ -x "${snykHome}/bin/snyk" ]; then
                SNYK_BIN="${snykHome}/bin/snyk"
              else
                echo "ERROR: Snyk binary not found in ${snykHome}"
                exit 127
              fi

              "\$SNYK_BIN" --version
              "\$SNYK_BIN" auth "\$SNYK_TOKEN"

              # deps should already exist from previous stage, but keep safe:
              test -d node_modules || npm install

              echo "[+] Snyk test -> snyk-report.json (do not fail build on vulns)"
              "\$SNYK_BIN" test --json --all-projects > snyk-report.json || true

              echo "[+] Snyk monitor (optional, also non-fatal)"
              "\$SNYK_BIN" monitor --all-projects --org=musfiqur-m || true

              ls -la snyk-report.json || true
            """
          }
        }
      }
    }

    // =========================
    // Task 4: Trivy FS JSON
    // =========================
    stage('File System Scan - Trivy (JSON)') {
      steps {
        sh '''
          set -eu
          echo "[+] Trivy fs -> trivy-fs.json (non-fatal)"
          trivy fs --format json --output trivy-fs.json . || true
          ls -la trivy-fs.json || true
        '''
      }
    }

    // =========================
    // Remove old deployment + prune unused images before next build
    // =========================
    stage('Docker Build & Push') {
      steps {
        script {
          withDockerRegistry(credentialsId: 'docker', toolName: 'docker') {
            sh """
              set -eu

              docker rm -f juice-shop || true
              docker image prune -af || true

              docker build -t juice-shop:latest .
              docker tag juice-shop:latest ${IMAGE}
              docker push ${IMAGE}
            """
          }
        }
      }
    }

    // =========================
    // Task 5: Trivy image HTML
    // =========================
    stage('Container Scan - Trivy (HTML)') {
      steps {
        sh '''
          set -eu
          echo "[+] Trivy image -> trivy-image.html (non-fatal)"

          curl -sSL https://raw.githubusercontent.com/aquasecurity/trivy/main/contrib/html.tpl -o html.tpl
          trivy image --format template --template "@html.tpl" --output trivy-image.html ${IMAGE} || true

          ls -la trivy-image.html || true
        '''
      }
    }

    stage('Deploy Container') {
      steps {
        sh '''
          set -eu
          docker rm -f juice-shop || true
          docker run -d --name juice-shop -p 3000:3000 ${IMAGE}
          # Give the app a moment to boot
          sleep 8
          docker ps --filter "name=juice-shop"
        '''
      }
    }

    // =========================
    // DAST - ZAP (fixed networking)
    // =========================
    stage('DAST - OWASP ZAP') {
      steps {
        sh '''
          set -eu
          chmod 777 "$(pwd)" || true

          echo "[+] Running ZAP using the Juice Shop container network namespace"
          docker run --rm \
            --network container:juice-shop \
            -v "$(pwd)":/zap/wrk/:rw \
            zaproxy/zap-stable \
            zap-baseline.py \
              -t http://127.0.0.1:3000/ \
              -I -j --auto \
              -r DAST_Report.html || true

          ls -la DAST_Report.html || true
        '''
      }
    }

  } // end stages

  // =========================
  // Task 6: Archive artifacts to build dashboard
  // =========================
  post {
    always {
      archiveArtifacts artifacts: '''
        semgrep-results.json,
        sbom.cdx.json,
        snyk-report.json,
        trivy-fs.json,
        trivy-image.html,
        DAST_Report.html
      ''', allowEmptyArchive: true, fingerprint: true

      sh 'docker rm -f juice-shop || true'
    }
  }

} // end pipeline
