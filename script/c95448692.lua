--ダメージ・ダイエット
function c95448692.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetOperation(c95448692.activate)
	c:RegisterEffect(e1)
	--effect damage
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetRange(LOCATION_GRAVE)
	e2:SetCost(c95448692.cost2)
	e2:SetOperation(c95448692.activate2)
	c:RegisterEffect(e2)
end
c95448692[0]=0
c95448692[1]=0
function c95448692.activate(e,tp,eg,ep,ev,re,r,rp)
	c95448692[tp]=1
	if Duel.GetFlagEffect(tp,95448692)~=0 then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CHANGE_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,0)
	e1:SetValue(c95448692.val)
	e1:SetReset(RESET_PHASE+PHASE_END,1)
	Duel.RegisterEffect(e1,tp)
	Duel.RegisterFlagEffect(tp,95448692,RESET_PHASE+PHASE_END,0,1)
end
function c95448692.cost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_EFFECT)
end
function c95448692.activate2(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFlagEffect(tp,95448692)~=0 then return end
	c95448692[tp]=0
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CHANGE_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,0)
	e1:SetValue(c95448692.val)
	e1:SetReset(RESET_PHASE+PHASE_END,1)
	Duel.RegisterEffect(e1,tp)
	Duel.RegisterFlagEffect(tp,95448692,RESET_PHASE+PHASE_END,0,1)
end
function c95448692.val(e,re,dam,r,rp,rc)
	if c95448692[e:GetOwnerPlayer()]==1 or bit.band(r,REASON_EFFECT)~=0 then
		return dam/2
	else return dam end
end
