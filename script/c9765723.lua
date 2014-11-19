--戦乙女の契約書
function c9765723.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetHintTiming(0,0x1c0+TIMING_DAMAGE_STEP)
	e1:SetCondition(c9765723.condition)
	e1:SetTarget(c9765723.target)
	e1:SetOperation(c9765723.operation)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(9765723,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(0,0x1c0)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1,9765723)
	e2:SetCost(c9765723.descost)
	e2:SetTarget(c9765723.destg)
	e2:SetOperation(c9765723.desop)
	c:RegisterEffect(e2)
	--atk
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,0)
	e3:SetCode(EFFECT_UPDATE_ATTACK)
	e3:SetCondition(c9765723.atkcon)
	e3:SetTarget(c9765723.atktg)
	e3:SetValue(1000)
	c:RegisterEffect(e3)
	--damage
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(9765723,1))
	e4:SetCategory(CATEGORY_DAMAGE)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e4:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCountLimit(1)
	e4:SetCondition(c9765723.damcon)
	e4:SetCost(c9765723.damcost)
	e4:SetTarget(c9765723.damtg)
	e4:SetOperation(c9765723.damop)
	c:RegisterEffect(e4)
end
function c9765723.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c9765723.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsDestructable() end
	if chk==0 then return true end
	local ph=Duel.GetCurrentPhase()
	local b1=Duel.GetFlagEffect(tp,9765723)==0
		and Duel.IsExistingMatchingCard(c9765723.cfilter,tp,LOCATION_HAND,0,1,nil)
		and Duel.IsExistingTarget(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
	local b2=Duel.GetTurnPlayer()==tp and ph==PHASE_STANDBY
	if (b1 or b2) and ph~=PHASE_DAMAGE and Duel.SelectYesNo(tp,aux.Stringid(9765723,2)) then
		local op=0
		if b1 and b2 then op=Duel.SelectOption(tp,aux.Stringid(9765723,0),aux.Stringid(9765723,1))
		elseif b1 then op=Duel.SelectOption(tp,aux.Stringid(9765723,0))
		else op=Duel.SelectOption(tp,aux.Stringid(9765723,1))+1 end
		e:SetLabel(op)
		if op==0 then
			e:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
			Duel.DiscardHand(tp,c9765723.cfilter,1,1,REASON_COST)
			Duel.RegisterFlagEffect(tp,9765723,RESET_PHASE+PHASE_END,0,1)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
			local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil)
			Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
		else
			e:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
			e:GetHandler():RegisterFlagEffect(9765723,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
			Duel.SetTargetPlayer(tp)
			Duel.SetTargetParam(1000)
			Duel.SetOperationInfo(0,CATEGORY_DAMAGE,0,0,tp,1000)
		end
	else
		e:SetLabel(2)
		e:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	end
end
function c9765723.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local op=e:GetLabel()
	if op==0 then
		local tc=Duel.GetFirstTarget()
		if tc:IsRelateToEffect(e) then
			Duel.Destroy(tc,REASON_EFFECT)
		end
	elseif op==1 then
		local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
		Duel.Damage(p,d,REASON_EFFECT)
	end
end
function c9765723.cfilter(c)
	return (c:IsSetCard(0xaf) or c:IsSetCard(0xae)) and c:IsAbleToGraveAsCost()
end
function c9765723.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,9765723)==0
		and Duel.IsExistingMatchingCard(c9765723.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c9765723.cfilter,1,1,REASON_COST)
	Duel.RegisterFlagEffect(tp,9765723,RESET_PHASE+PHASE_END,0,1)
end
function c9765723.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c9765723.desop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c9765723.atkcon(e)
	return Duel.GetTurnPlayer()~=e:GetHandlerPlayer()
end
function c9765723.atktg(e,c)
	return c:IsRace(RACE_FIEND)
end
function c9765723.damcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c9765723.damcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(9765723)==0 end
	e:GetHandler():RegisterFlagEffect(9765723,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c9765723.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1000)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,0,0,tp,1000)
end
function c9765723.damop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
