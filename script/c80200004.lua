--幻奏の音女カノン
function c80200004.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCountLimit(1,80200004)
	e1:SetCondition(c80200004.spcon)
	c:RegisterEffect(e1)	
	--swap
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(80200004,1))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1)
	e2:SetTarget(c80200004.tg)
	e2:SetOperation(c80200004.op)
	c:RegisterEffect(e2)
end
function c80200004.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x9b)
end
function c80200004.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c80200004.filter,tp,LOCATION_MZONE,0,1,nil)
end
function c80200004.tg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) end
	if chk==0 then return Duel.IsExistingTarget(nil,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c80200004.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c80200004.op(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.ChangePosition(tc,POS_FACEUP_DEFENCE,0,POS_FACEUP_ATTACK,0)
	end
end