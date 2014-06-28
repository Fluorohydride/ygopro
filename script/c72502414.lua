--エクシーズ・エージェント
function c72502414.initial_effect(c)
	--material
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(72502414,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCountLimit(1,72502414+EFFECT_COUNT_CODE_DUEL)
	e1:SetTarget(c72502414.target)
	e1:SetOperation(c72502414.operation)
	c:RegisterEffect(e1)
end
function c72502414.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x7f) and c:IsType(TYPE_XYZ)
end
function c72502414.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c72502414.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c72502414.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c72502414.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c72502414.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) and not tc:IsImmuneToEffect(e) then
		Duel.Overlay(tc,Group.FromCards(c))
	end
end
