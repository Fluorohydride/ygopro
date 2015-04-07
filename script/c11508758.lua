--ミュータント・ハイブレイン
function c11508758.initial_effect(c)
	--control
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(11508758,0))
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c11508758.ctlcon)
	e1:SetTarget(c11508758.ctltg)
	e1:SetOperation(c11508758.ctlop)
	c:RegisterEffect(e1)
end
function c11508758.ctlcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetAttackTarget()~=nil
end
function c11508758.filter(c)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsControlerCanBeChanged()
end
function c11508758.ctltg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc~=Duel.GetAttackTarget() and c11508758.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c11508758.filter,tp,0,LOCATION_MZONE,1,Duel.GetAttackTarget()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
	local g=Duel.SelectTarget(tp,c11508758.filter,tp,0,LOCATION_MZONE,1,1,Duel.GetAttackTarget())
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,1,0,0)
end
function c11508758.ctlop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		if not Duel.GetControl(tc,tp,PHASE_BATTLE,1) then
			if not tc:IsImmuneToEffect(e) and tc:IsAbleToChangeControler() then
				Duel.Destroy(tc,REASON_EFFECT)
			end
		else
			if tc:IsAttackPos() then
				Duel.ChangeAttacker(tc)
			end
		end
	end
end
