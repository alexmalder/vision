deploy:
	ansible-playbook \
		./.ci/playbook.yml \
		-e "pg_user=${POSTGRES_USER}" \
		-e "pg_pass=${POSTGRES_PASSWORD}" \
		-e "pg_host=redash-postgresql" \
		-e "pg_port=5432 pg_db=${POSTGRES_DB}"
