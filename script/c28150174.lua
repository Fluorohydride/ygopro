--フォトン・バタフライ・アサシン
function c28150174.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,4,2)
	c:EnableReviveLimit()
	--pos&atk
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(28150174,0))
	e1:SetCategory(CATEGORY_POSITION+CATEGORY_ATKCHANGE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c28150174.poscost)
	e1:SetTarget(c28150174.postg)
	e1:SetOperation(c28150174.posop)
	c:RegisterEffect(e1)
end
function c28150174.poscost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c28150174.postg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsDefencePos() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDefencePos,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DEFENCE)
	local g=Duel.SelectTarget(tp,Card.IsDefencePos,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
end
function c28150174.posop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsDefencePos() and tc:IsRelateToEffect(e) then
		Duel.ChangePosition(tc,POS_FACEUP_ATTACK)
		if tc:IsPosition(POS_FACEUP_ATTACK) then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetValue(-600)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
		end
	end
end
