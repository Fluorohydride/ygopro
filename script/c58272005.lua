--生存競争
function c58272005.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DAMAGE_STEP)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCondition(c58272005.condition)
	e1:SetTarget(c58272005.target)
	e1:SetOperation(c58272005.operation)
	c:RegisterEffect(e1)
end
function c58272005.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c58272005.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_DINOSAUR)
end
function c58272005.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c58272005.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c58272005.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	local g=Duel.SelectTarget(tp,c58272005.filter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,e:GetHandler(),1,0,0)
end
function c58272005.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsLocation(LOCATION_SZONE) then return end
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Equip(tp,c,tc)
		c:CancelToGrave()
		--Atkup
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_EQUIP)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(1000)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
		--Equip limit
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_EQUIP_LIMIT)
		e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e2:SetValue(c58272005.eqlimit)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e2)
		--chain attack
		local e3=Effect.CreateEffect(c)
		e3:SetDescription(aux.Stringid(58272005,0))
		e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
		e3:SetRange(LOCATION_SZONE)
		e3:SetCode(EVENT_BATTLE_DESTROYING)
		e3:SetCondition(c58272005.atcon)
		e3:SetOperation(c58272005.atop)
		e3:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e3)
	end
end
function c58272005.eqlimit(e,c)
	return c:IsRace(RACE_DINOSAUR)
end
function c58272005.atcon(e,tp,eg,ep,ev,re,r,rp)
	local ec=e:GetHandler():GetEquipTarget()
	if not eg:IsContains(ec) then return false end
	local bc=ec:GetBattleTarget()
	return bc:IsLocation(LOCATION_GRAVE) and bc:IsType(TYPE_MONSTER) and ec:IsChainAttackable(2,true) and ec:IsStatus(STATUS_OPPO_BATTLE)
end
function c58272005.atop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ec=e:GetHandler():GetEquipTarget()
	if not ec:IsRelateToBattle() then return end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetCode(EFFECT_EXTRA_ATTACK)
	e1:SetValue(1)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_BATTLE)
	ec:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_BATTLE)
	ec:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_ATTACK_ANNOUNCE)
	e3:SetLabelObject(e1)
	e3:SetOperation(aux.atrst)
	e3:SetReset(RESET_PHASE+PHASE_BATTLE)
	Duel.RegisterEffect(e3,tp)
end
