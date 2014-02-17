--進化の宿命
function c8632967.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--act limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetOperation(c8632967.sucop)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_CHAIN_END)
	e3:SetOperation(c8632967.cedop)
	e3:SetLabelObject(e2)
	c:RegisterEffect(e3)
end
function c8632967.chainlm(e,rp,tp)
	return tp==rp
end
function c8632967.sucfilter(c)
	local st=c:GetSummonType()
	return c:IsSetCard(0x304e) and st>=(SUMMON_TYPE_SPECIAL+150) and st<(SUMMON_TYPE_SPECIAL+180)
end
function c8632967.sucop(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(c8632967.sucfilter,1,nil) then
		e:SetLabel(1)
	else e:SetLabel(0) end
end
function c8632967.cedop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.CheckEvent(EVENT_SPSUMMON_SUCCESS) and e:GetLabelObject():GetLabel()==1 then
		Duel.SetChainLimitTillChainEnd(c8632967.chainlm)
	end
end
