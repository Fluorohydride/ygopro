--インヴェルズの門番
function c26722601.initial_effect(c)
	--extra summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetOperation(c26722601.sumop)
	c:RegisterEffect(e1)
end
function c26722601.sumop(e,tp,eg,ep,ev,re,r,rp)
	local ec=eg:GetFirst()
	local c=e:GetHandler()
	if ec~=e:GetHandler() and ec:IsSetCard(0x100a) and ec:GetSummonType()==SUMMON_TYPE_ADVANCE then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetRange(LOCATION_MZONE)
		e1:SetTargetRange(LOCATION_HAND+LOCATION_MZONE,0)
		e1:SetCode(EFFECT_EXTRA_SUMMON_COUNT)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_EXTRA_SET_COUNT)
		c:RegisterEffect(e2)
	end
end
