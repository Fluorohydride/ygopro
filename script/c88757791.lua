--法眼の魔術師
function c88757791.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--change scale
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_PZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1)
	e2:SetTarget(c88757791.sctg)
	e2:SetOperation(c88757791.scop)
	c:RegisterEffect(e2)
	--indes
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e3:SetRange(LOCATION_MZONE)
	e3:SetTargetRange(LOCATION_MZONE,0)
	e3:SetCondition(c88757791.indcon)
	e3:SetTarget(c88757791.indtg)
	e3:SetValue(c88757791.indval)
	c:RegisterEffect(e3)
end
function c88757791.cfilter(c,sc)
	return c:IsType(TYPE_PENDULUM) and not c:IsPublic()
		and ((sc:GetSequence()==6 and c:GetLeftScale()~=sc:GetLeftScale())
		or (sc:GetSequence()==7 and c:GetRightScale()~=sc:GetRightScale()))
end
function c88757791.scfilter(c,tp)
	return c:IsSetCard(0x98) and (c:GetSequence()==6 or c:GetSequence()==7)
		and Duel.IsExistingMatchingCard(c88757791.cfilter,tp,LOCATION_HAND,0,1,nil,c)
end
function c88757791.sctg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and chkc:IsControler(tp) and c88757791.scfilter(chkc,tp) end
	if chk==0 then return Duel.IsExistingTarget(c88757791.scfilter,tp,LOCATION_SZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=Duel.SelectTarget(tp,c88757791.scfilter,tp,LOCATION_SZONE,0,1,1,nil,tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local cg=Duel.SelectMatchingCard(tp,c88757791.cfilter,tp,LOCATION_HAND,0,1,1,nil,g:GetFirst())
	Duel.ConfirmCards(1-tp,cg)
	Duel.ShuffleHand(tp)
	e:SetLabelObject(cg:GetFirst())
end
function c88757791.scop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_LSCALE)
		e1:SetValue(e:GetLabelObject():GetLeftScale())
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
		tc:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_CHANGE_RSCALE)
		e2:SetValue(e:GetLabelObject():GetRightScale())
		tc:RegisterEffect(e2)
	end
end
function c88757791.indcon(e)
	local c=e:GetHandler()
	return c:IsStatus(STATUS_SUMMON_TURN) and c:GetSummonType()==SUMMON_TYPE_PENDULUM
end
function c88757791.indtg(e,c)
	return c:IsSetCard(0x98) and c:IsType(TYPE_PENDULUM)
end
function c88757791.indval(e,re,rp)
	return rp~=e:GetHandlerPlayer()
end
