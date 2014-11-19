--ゴーストリック・キョンシー
function c80885284.initial_effect(c)
	--summon limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetCondition(c80885284.sumcon)
	c:RegisterEffect(e1)
	--turn set
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(80885284,0))
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTarget(c80885284.postg)
	e2:SetOperation(c80885284.posop)
	c:RegisterEffect(e2)
	--search
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(80885284,1))
	e3:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e3:SetCode(EVENT_FLIP)
	e3:SetCountLimit(1,80885284)
	e3:SetTarget(c80885284.thtg)
	e3:SetOperation(c80885284.thop)
	c:RegisterEffect(e3)
end
function c80885284.sfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x8d)
end
function c80885284.sumcon(e)
	return not Duel.IsExistingMatchingCard(c80885284.sfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c80885284.postg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsCanTurnSet() and c:GetFlagEffect(80885284)==0 end
	c:RegisterFlagEffect(80885284,RESET_EVENT+0x1fc0000+RESET_PHASE+PHASE_END,0,1)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,c,1,0,0)
end
function c80885284.posop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		Duel.ChangePosition(c,POS_FACEDOWN_DEFENCE)
	end
end
function c80885284.thfilter(c,lv)
	return c:IsLevelBelow(lv) and c:IsSetCard(0x8d) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c80885284.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local count=Duel.GetMatchingGroupCount(c80885284.sfilter,tp,LOCATION_MZONE,0,nil)
		return Duel.IsExistingMatchingCard(c80885284.thfilter,tp,LOCATION_DECK,0,1,nil,count)
	end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c80885284.thop(e,tp,eg,ep,ev,re,r,rp)
	local count=Duel.GetMatchingGroupCount(c80885284.sfilter,tp,LOCATION_MZONE,0,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c80885284.thfilter,tp,LOCATION_DECK,0,1,1,nil,count)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
