--ゼクト・コンバージョン
function c74854609.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BE_BATTLE_TARGET)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCondition(c74854609.condition)
	e1:SetTarget(c74854609.target)
	e1:SetOperation(c74854609.operation)
	c:RegisterEffect(e1)
end
function c74854609.condition(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return d:IsFaceup() and d:IsSetCard(0x56) and a:IsControler(1-tp)
end
function c74854609.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local a=Duel.GetAttacker()
	if chkc then return a==chkc end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_SZONE)>0
		and a:IsOnField() and a:IsCanBeEffectTarget(e) end
	Duel.SetTargetCard(a)
end
function c74854609.eqlimit(e,c)
	return c==e:GetLabelObject()
end
function c74854609.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ec=Duel.GetAttackTarget()
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsAttackable() and not tc:IsStatus(STATUS_ATTACK_CANCELED)
		and ec:IsLocation(LOCATION_MZONE) and ec:IsFaceup() then
		Duel.Equip(tp,ec,tc)
		--Add Equip limit
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_EQUIP_LIMIT)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(c74854609.eqlimit)
		e1:SetLabelObject(tc)
		ec:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_EQUIP)
		e2:SetCode(EFFECT_SET_CONTROL)
		e2:SetValue(tp)
		e2:SetReset(RESET_EVENT+0x1fc0000)
		ec:RegisterEffect(e2)
	end
end
