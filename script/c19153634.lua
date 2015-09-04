--ノーブル・ド・ノワール
function c19153634.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c19153634.condition)
	e1:SetOperation(c19153634.operation)
	c:RegisterEffect(e1)
end
function c19153634.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer()	
end
function c19153634.operation(e,tp,eg,ep,ev,re,r,rp)
	local ats=eg:GetFirst():GetAttackableTarget()
	local at=Duel.GetAttackTarget()
	if ats:GetCount()==0 or (at and ats:GetCount()==1) then return end
	if Duel.SelectYesNo(tp,aux.Stringid(19153634,0)) then
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(19153634,1))
		local g=ats:Select(tp,1,1,at)
		Duel.Hint(HINT_CARD,0,19153634)
		Duel.HintSelection(g)
		Duel.ChangeAttackTarget(g:GetFirst())
	end
end
