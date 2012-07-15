--統制訓練
function c85352446.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_ATTACK,0x11e0)
	e1:SetTarget(c85352446.target)
	e1:SetOperation(c85352446.activate)
	c:RegisterEffect(e1)
end
function c85352446.filter(c)
	local lv=c:GetLevel()
	return lv>0 and lv<=5 and c:IsFaceup() and Duel.IsExistingMatchingCard(c85352446.filter2,0,LOCATION_MZONE,LOCATION_MZONE,1,c,lv)
end
function c85352446.filter2(c,lv)
	local clv=c:GetLevel()
	return c:IsFaceup() and clv>0 and lv~=clv and c:IsDestructable()
end
function c85352446.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c85352446.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c85352446.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c85352446.filter,tp,0,LOCATION_MZONE,1,1,nil)
	local dg=Duel.GetMatchingGroup(c85352446.filter2,0,LOCATION_MZONE,LOCATION_MZONE,nil,g:GetFirst():GetLevel())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c85352446.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local dg=Duel.GetMatchingGroup(c85352446.filter2,0,LOCATION_MZONE,LOCATION_MZONE,tc,tc:GetLevel())
		Duel.Destroy(dg,REASON_EFFECT)
	end
end
