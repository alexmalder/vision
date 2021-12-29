#!/bin/bash
set -eo pipefail

ansible-playbook \
	./.ci/playbook.yml \
	-e "pg_user=${POSTGRES_USER}" \
	-e "pg_pass=${POSTGRES_PASSWORD}" \
	-e "pg_host=redash-postgresql" \
	-e "pg_port=5432" \
	-e "pg_db=${POSTGRES_DB}" \
	-e "tag=0.1.3"
