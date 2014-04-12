--ヒロイック・リベンジ・ソード
function c49551909.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_BATTLE_PHASE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c49551909.target)
	e1:SetOperation(c49551909.operation)
	c:RegisterEffect(e1)
end
function c49551909.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x6f)
end
function c49551909.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c49551909.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c49551909.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	local g=Duel.SelectTarget(tp,c49551909.filter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,e:GetHandler(),1,0,0)
end
function c49551909.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsLocation(LOCATION_SZONE) then return end
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Equip(tp,c,tc)
		c:CancelToGrave()
		--destroy
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
		e1:SetCategory(CATEGORY_DESTROY)
		e1:SetDescription(aux.Stringid(49551909,0))
		e1:SetCode(EVENT_BATTLED)
		e1:SetRange(LOCATION_SZONE)
		e1:SetCondition(c49551909.descon)
		e1:SetTarget(c49551909.destg)
		e1:SetOperation(c49551909.desop)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
		--damage
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_PRE_BATTLE_DAMAGE)
		e2:SetRange(LOCATION_SZONE)
		e2:SetCondition(c49551909.damcon)
		e2:SetOperation(c49551909.damop)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e2)
		--Equip limit
		local e3=Effect.CreateEffect(c)
		e3:SetType(EFFECT_TYPE_SINGLE)
		e3:SetCode(EFFECT_EQUIP_LIMIT)
		e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e3:SetValue(c49551909.eqlimit)
		e3:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e3)
	end
end
function c49551909.eqlimit(e,c)
	return c:GetControler()==e:GetOwnerPlayer() and c:IsSetCard(0x6f)
end
function c49551909.descon(e,tp,eg,ep,ev,re,r,rp)
	local ec=e:GetHandler():GetEquipTarget()
	return Duel.GetAttackTarget()==ec or (Duel.GetAttacker()==ec and Duel.GetAttackTarget())
end
function c49551909.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,e:GetHandler():GetEquipTarget():GetBattleTarget(),1,0,0)
end
function c49551909.desop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local bc=e:GetHandler():GetEquipTarget():GetBattleTarget()
	if bc:IsRelateToBattle() then
		Duel.Destroy(bc,REASON_EFFECT)
	end
end
function c49551909.damcon(e,tp,eg,ep,ev,re,r,rp)
	local ec=e:GetHandler():GetEquipTarget()
	return ec and ep==tp and (Duel.GetAttacker()==ec or Duel.GetAttackTarget()==ec)
end
function c49551909.damop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeBattleDamage(1-tp,ev,false)
end
