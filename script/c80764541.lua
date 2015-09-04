--No.44 白天馬スカイ・ペガサス
function c80764541.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,4,2)
	c:EnableReviveLimit()
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetDescription(aux.Stringid(80764541,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c80764541.descost)
	e1:SetTarget(c80764541.destg)
	e1:SetOperation(c80764541.desop)
	c:RegisterEffect(e1)
end
c80764541.xyz_number=44
function c80764541.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c80764541.filter(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c80764541.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c80764541.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c80764541.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c80764541.filter,tp,0,LOCATION_MZONE,1,1,nil)
	if Duel.GetLP(1-tp)>1000 then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	end
end
function c80764541.desop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.CheckLPCost(1-tp,1000) and Duel.SelectYesNo(1-tp,aux.Stringid(80764541,1)) then
		Duel.PayLPCost(1-tp,1000)
		if Duel.IsChainDisablable(0) then
			Duel.NegateEffect(0)
			return
		end
	end
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
