--古代の機械工場
function c30435145.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c30435145.target)
	e1:SetOperation(c30435145.operation)
	c:RegisterEffect(e1)
end
function c30435145.filter(c,g)
	return c:GetLevel()>4 and c:IsSetCard(0x7) and g:CheckWithSumEqual(Card.GetLevel,c:GetLevel()*2,1,99)
end
function c30435145.rfilter(c)
	return c:GetLevel()>0 and c:IsSetCard(0x7) and c:IsAbleToRemove()
end
function c30435145.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local rg=Duel.GetMatchingGroup(c30435145.rfilter,tp,LOCATION_GRAVE,0,nil)
		return Duel.IsExistingMatchingCard(c30435145.filter,tp,LOCATION_HAND,0,1,nil,rg)
	end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,1,tp,LOCATION_GRAVE)
end
function c30435145.operation(e,tp,eg,ep,ev,re,r,rp)
	local rg=Duel.GetMatchingGroup(c30435145.rfilter,tp,LOCATION_GRAVE,0,nil)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(30435145,1))
	local g=Duel.SelectMatchingCard(tp,c30435145.filter,tp,LOCATION_HAND,0,1,1,nil,rg)
	local tc=g:GetFirst()
	if tc then
		Duel.ConfirmCards(1-tp,tc)
		Duel.ShuffleHand(tp)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local sg=rg:SelectWithSumEqual(tp,Card.GetLevel,tc:GetLevel()*2,1,99)
		Duel.Remove(sg,POS_FACEUP,REASON_EFFECT)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetDescription(aux.Stringid(30435145,0))
		e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SUMMON_PROC)
		e1:SetCondition(c30435145.ntcon)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
	end
end
function c30435145.ntcon(e,c,minc)
	if c==nil then return true end
	return minc==0 and c:GetLevel()>4 and Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
end
