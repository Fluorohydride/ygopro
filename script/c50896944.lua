--暗黒ブラキ
function c50896944.initial_effect(c)
	--pos
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(50896944,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c50896944.postg)
	e1:SetOperation(c50896944.posop)
	c:RegisterEffect(e1)
end
function c50896944.filter(c)
	return not c:IsPosition(POS_FACEUP_DEFENCE)
end
function c50896944.postg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c50896944.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c50896944.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_POSCHANGE)
	Duel.SelectTarget(tp,c50896944.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c50896944.posop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and not tc:IsPosition(POS_FACEUP_DEFENCE) then
		Duel.ChangePosition(tc,POS_FACEUP_DEFENCE)
	end
end
