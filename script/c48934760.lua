--終焉の地
function c48934760.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c48934760.condition)
	e1:SetTarget(c48934760.target)
	e1:SetOperation(c48934760.activate)
	c:RegisterEffect(e1)
end
function c48934760.cfilter(c,tp)
	return c:GetSummonPlayer()==1-tp
end
function c48934760.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c48934760.cfilter,1,nil,tp)
end
function c48934760.filter(c,tp)
	return c:IsType(TYPE_FIELD) and c:GetActivateEffect():IsActivatable(tp)
end
function c48934760.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c48934760.filter,tp,LOCATION_DECK,0,1,nil,tp) end
end
function c48934760.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(48934760,0))
	local tc=Duel.SelectMatchingCard(tp,c48934760.filter,tp,LOCATION_DECK,0,1,1,nil,tp):GetFirst()
	if tc then
		local op=tc:GetActivateEffect():GetOperation()
		if Duel.GetFieldCard(tp,LOCATION_SZONE,5)~=nil then
			Duel.Destroy(Duel.GetFieldCard(tp,LOCATION_SZONE,5),REASON_RULE)
			Duel.BreakEffect()
			Duel.MoveToField(tc,tp,tp,LOCATION_SZONE,POS_FACEUP,true)
			if op then op(e,tp,eg,ep,ev,re,r,rp) end
		elseif Duel.GetFieldCard(1-tp,LOCATION_SZONE,5)~=nil
			and Duel.GetFieldCard(1-tp,LOCATION_SZONE,5):IsFaceup() then
			Duel.MoveToField(tc,tp,tp,LOCATION_SZONE,POS_FACEUP,true)
			if op then op(e,tp,eg,ep,ev,re,r,rp) end
			Duel.Destroy(Duel.GetFieldCard(1-tp,LOCATION_SZONE,5),REASON_RULE)
		else
			Duel.MoveToField(tc,tp,tp,LOCATION_SZONE,POS_FACEUP,true)
			if op then op(e,tp,eg,ep,ev,re,r,rp) end
		end
	end
end
