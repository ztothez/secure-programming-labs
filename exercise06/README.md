# Exercise 06 – Secure CI/CD Pipeline (Jenkins)

This exercise contains a Jenkins pipeline that integrates multiple security tools into a CI workflow.

The pipeline performs:

- SAST scanning (Semgrep)
- SBOM generation (Syft)
- SCA scanning (Snyk)
- Filesystem and container scanning (Trivy)
- DAST scanning (OWASP ZAP)
- Docker build and deployment

Security reports are archived as build artifacts.

The goal of this exercise is to demonstrate how security testing can be integrated into automated CI/CD pipelines.
