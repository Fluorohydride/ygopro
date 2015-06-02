--フライファング
function c11954712.initial_effect(c)
	--reg
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetOperation(c11954712.regop)
	c:RegisterEffect(e1)
	--pierce
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_PIERCE)
	c:RegisterEffect(e2)
	--remove
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(11954712,0))
	e3:SetCategory(CATEGORY_REMOVE)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EVENT_PHASE+PHASE_BATTLE)
	e3:SetCountLimit(1)
	e3:SetCondition(c11954712.rmcon)
	e3:SetTarget(c11954712.rmtg)
	e3:SetOperation(c11954712.rmop)
	c:RegisterEffect(e3)
end
function c11954712.regop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RegisterFlagEffect(11954712,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_BATTLE,0,1)
end
function c11954712.rmcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(11954712)~=0
end
function c11954712.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,e:GetHandler(),1,0,0)
end
function c11954712.rmop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		if Duel.Remove(c,0,REASON_EFFECT+REASON_TEMPORARY)==0 then return end
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
		e1:SetCountLimit(1)
		e1:SetLabelObject(c)
		e1:SetCondition(c11954712.retcon)
		e1:SetOperation(c11954712.retop)
		e1:SetReset(RESET_PHASE+PHASE_STANDBY+RESET_SELF_TURN)
		Duel.RegisterEffect(e1,tp)
	end
end
function c11954712.retcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c11954712.retop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ReturnToField(e:GetLabelObject())
end
