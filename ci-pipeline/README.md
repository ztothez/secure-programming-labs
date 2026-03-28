# Secure CI/CD Pipeline (Jenkins)
Implements a security-focused CI/CD pipeline that integrates multiple analysis tools into an automated build and deployment workflow.

## Overview
This module demonstrates how security testing can be embedded into continuous integration pipelines. The pipeline performs static analysis, dependency scanning, artifact inspection, container security checks, and dynamic testing before deployment.

The goal is to provide continuous visibility into application security throughout the development lifecycle.

## Pipeline Features
- Static Application Security Testing (SAST) using Semgrep  
- Software Bill of Materials (SBOM) generation using Syft  
- Software Composition Analysis (SCA) using Snyk  
- Filesystem vulnerability scanning using Trivy  
- Container image scanning using Trivy  
- Dynamic Application Security Testing (DAST) using OWASP ZAP  
- Docker build, tagging, and deployment  

## Workflow
1. Clean workspace and fetch source code  
2. Perform static code analysis (Semgrep)  
3. Generate SBOM (Syft)  
4. Run dependency vulnerability scans (Snyk)  
5. Scan filesystem for vulnerabilities (Trivy)  
6. Build and push Docker image  
7. Scan container image (Trivy)  
8. Deploy container  
9. Perform dynamic security testing (OWASP ZAP)  
10. Archive security reports as build artifacts  

## Implementation
- `jenkins_pipeline.groovy` — Jenkins pipeline defining the full CI/CD workflow  

## Usage
Run the pipeline in a Jenkins environment with:

- Docker installed  
- Required tools configured (Snyk, Node.js, JDK)  
- Credentials set (Docker registry, Snyk token)  

The pipeline will automatically:
- Execute all security checks  
- Deploy the application  
- Generate and archive reports  

## Security Considerations
- Security scans are integrated directly into the build process  
- Vulnerability detection does not block the pipeline by default (non-fatal scans)  
- Reports are preserved as artifacts for analysis and auditing  
- Combines multiple scanning layers (code, dependencies, container, runtime)  

## Purpose
Part of a broader exploration of secure software development and DevSecOps practices.