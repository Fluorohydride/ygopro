--CNo.73 激瀧瀑神アビス・スープラ
function c96864105.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,6,3)
	c:EnableReviveLimit()
	--atk up
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(96864105,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetRange(LOCATION_MZONE)
	e1:SetHintTiming(TIMING_DAMAGE_CAL)
	e1:SetCondition(c96864105.atkcon)
	e1:SetCost(c96864105.atkcost)
	e1:SetOperation(c96864105.atkop)
	c:RegisterEffect(e1)
	--indes
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e2:SetCondition(c96864105.indcon)
	e2:SetValue(1)
	c:RegisterEffect(e2)
end
c96864105.xyz_number=73
function c96864105.atkcon(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return d and a:GetControler()~=d:GetControler()
		and Duel.GetCurrentPhase()==PHASE_DAMAGE_CAL and not Duel.IsDamageCalculated()
end
function c96864105.atkcost(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:CheckRemoveOverlayCard(tp,1,REASON_COST) and c:GetFlagEffect(96864105)==0 end
	c:RemoveOverlayCard(tp,1,1,REASON_COST)
	c:RegisterFlagEffect(96864105,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DAMAGE_CAL,0,1)
end
function c96864105.atkop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if not a:IsRelateToBattle() or a:IsFacedown() or not d:IsRelateToBattle() or d:IsFacedown() then return end
	if a:IsControler(1-tp) then a,d=d,a end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetOwnerPlayer(tp)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_DAMAGE_CAL)
	e1:SetValue(d:GetAttack())
	a:RegisterEffect(e1)
end
function c96864105.indcon(e)
	return e:GetHandler():GetOverlayGroup():IsExists(Card.IsCode,1,nil,36076683)
end
