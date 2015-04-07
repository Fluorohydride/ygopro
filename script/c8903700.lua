--儀式魔人リリーサー
function c8903700.initial_effect(c)
	--ritual material
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EXTRA_RITUAL_MATERIAL)
	c:RegisterEffect(e1)
	--become material
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCode(EVENT_BE_MATERIAL)
	e2:SetCondition(c8903700.condition)
	e2:SetOperation(c8903700.operation)
	c:RegisterEffect(e2)
end
function c8903700.condition(e,tp,eg,ep,ev,re,r,rp)
	return r==REASON_RITUAL
end
function c8903700.operation(e,tp,eg,ep,ev,re,r,rp)
	local rc=e:GetHandler():GetReasonCard()
	if rc:GetFlagEffect(8903700)==0 then
		--cannot special summon
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetDescription(aux.Stringid(8903700,0))
		e1:SetProperty(EFFECT_FLAG_CLIENT_HINT+EFFECT_FLAG_PLAYER_TARGET)
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetAbsoluteRange(rp,0,1)
		rc:RegisterEffect(e1,true)
		rc:RegisterFlagEffect(8903700,RESET_EVENT+0x1fe0000,0,1)
	end
end
