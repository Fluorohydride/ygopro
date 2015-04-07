--レアメタル化・魔法反射装甲
function c12503902.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DISABLE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DAMAGE_STEP)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCondition(c12503902.condition)
	e1:SetTarget(c12503902.target)
	e1:SetOperation(c12503902.operation)
	c:RegisterEffect(e1)
	--Destroy
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_LEAVE_FIELD)
	e3:SetCondition(c12503902.descon2)
	e3:SetOperation(c12503902.desop2)
	c:RegisterEffect(e3)
end
function c12503902.descon2(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetFirstCardTarget()
	return tc and eg:IsContains(tc)
end
function c12503902.desop2(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(),REASON_EFFECT)
end
function c12503902.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c12503902.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_MACHINE)
end
function c12503902.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c12503902.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c12503902.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c12503902.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c12503902.tfilter1(c,tc)
	return c:IsType(TYPE_SPELL) and c:IsHasCardTarget(tc)
end
function c12503902.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and c12503902.filter(tc) and tc:IsRelateToEffect(e) then
		c:SetCardTarget(tc)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_OWNER_RELATE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetCondition(c12503902.rcon)
		e1:SetValue(500)
		tc:RegisterEffect(e1,true)
		local g=Duel.GetMatchingGroup(c12503902.tfilter1,tp,LOCATION_SZONE,LOCATION_SZONE,nil,tc)
		if g:GetCount()>0 then
			local sg,fid=g:GetMaxGroup(Card.GetFieldID)
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_FIELD)
			e1:SetRange(LOCATION_SZONE)
			e1:SetTargetRange(LOCATION_SZONE,LOCATION_SZONE)
			e1:SetCode(EFFECT_DISABLE)
			e1:SetCondition(c12503902.discon)
			e1:SetTarget(c12503902.distg)
			e1:SetLabel(fid)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			c:RegisterEffect(e1,true)
		else
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
			e1:SetRange(LOCATION_SZONE)
			e1:SetCode(EVENT_CHAIN_SOLVING)
			e1:SetCondition(c12503902.discon2)
			e1:SetOperation(c12503902.disop2)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			c:RegisterEffect(e1,true)
		end
	end
end
function c12503902.rcon(e)
	return e:GetOwner():IsHasCardTarget(e:GetHandler())
end
function c12503902.discon(e)
	return e:GetHandler():GetCardTargetCount()>0
end
function c12503902.distg(e,c)
	return c:GetFieldID()<=e:GetLabel() and c:IsHasCardTarget(e:GetHandler():GetFirstCardTarget()) and c:IsType(TYPE_SPELL)
end
function c12503902.discon2(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetFirstCardTarget()
	if not tc or not re:IsActiveType(TYPE_SPELL) or not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then return false end
	local g=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
	return g:IsContains(tc)
end
function c12503902.disop2(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateEffect(ev)
	e:Reset()
end
