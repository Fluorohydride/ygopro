--「A」細胞散布爆弾
function c73262676.initial_effect(c)
	--counter
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_COUNTER+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c73262676.target)
	e1:SetOperation(c73262676.operation)
	c:RegisterEffect(e1)
end
function c73262676.filter(c)
	return c:IsFaceup() and c:IsSetCard(0xc) and c:GetLevel()>0 and c:IsDestructable()
end
function c73262676.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c73262676.filter(chkc) end 
	if chk==0 then return Duel.IsExistingTarget(c73262676.filter,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingMatchingCard(Card.IsFaceup,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c73262676.filter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c73262676.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local lv=tc:GetLevel()
		if Duel.Destroy(tc,REASON_EFFECT)==0 then return end
		local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,0,LOCATION_MZONE,nil)
		if g:GetCount()==0 then return end
		for i=1,lv do
			local sg=g:Select(tp,1,1,nil)
			sg:GetFirst():AddCounter(0xe,1)
		end
	end
end
