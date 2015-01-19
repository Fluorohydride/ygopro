--マジックアーム・シールド
function c96008713.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c96008713.condition)
	e1:SetTarget(c96008713.target)
	e1:SetOperation(c96008713.activate)
	c:RegisterEffect(e1)
end
function c96008713.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer() and Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)>0
end
function c96008713.filter(c)
	return c:IsFaceup() and c:IsControlerCanBeChanged()
end
function c96008713.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local atr=Duel.GetAttacker()
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c96008713.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c96008713.filter,tp,0,LOCATION_MZONE,1,atr) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
	local g=Duel.SelectTarget(tp,c96008713.filter,tp,0,LOCATION_MZONE,1,1,atr)
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,1,0,0)
end
function c96008713.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local a=Duel.GetAttacker()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		if not Duel.GetControl(tc,tp,PHASE_BATTLE,1) then
			if not tc:IsImmuneToEffect(e) and tc:IsAbleToChangeControler() then
				Duel.Destroy(tc,REASON_EFFECT)
			end
		elseif a:IsAttackable() and not a:IsImmuneToEffect(e) then
			Duel.CalculateDamage(a,tc)
		end
	end
end
