--スケイルモース
function c68087897.initial_effect(c)
	--disable spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_SPSUMMON_COUNT_LIMIT)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,1)
	e1:SetValue(c68087897.spval)
	c:RegisterEffect(e1)
	--reg
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c68087897.spreg)
	c:RegisterEffect(e2)
end
function c68087897.spval(e,se,sp)
	return 1-e:GetHandler():GetFlagEffect(68087897+sp)
end
function c68087897.filter(c,tp)
	return c:GetSummonPlayer()==tp
end
function c68087897.spreg(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(c68087897.filter,1,nil,tp) then
		e:GetHandler():RegisterFlagEffect(68087897+tp,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	end
	if eg:IsExists(c68087897.filter,1,nil,1-tp) then
		e:GetHandler():RegisterFlagEffect(68087897+1-tp,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	end
end
