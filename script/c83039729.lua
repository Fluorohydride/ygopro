--六武衆の師範
function c83039729.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c83039729.spcon)
	c:RegisterEffect(e1)
	--to hand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(83039729,0))
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_DESTROYED)
	e2:SetCondition(c83039729.thcon)
	e2:SetTarget(c83039729.thtg)
	e2:SetOperation(c83039729.thop)
	c:RegisterEffect(e2)
	--only 1 can exists
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_SUMMON)
	e3:SetCondition(c83039729.excon)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e5:SetCode(EFFECT_SPSUMMON_CONDITION)
	e5:SetValue(c83039729.splimit)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetCode(EFFECT_SELF_DESTROY)
	e6:SetCondition(c83039729.descon)
	c:RegisterEffect(e6)
end
function c83039729.spfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x3d)
end
function c83039729.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and	Duel.IsExistingMatchingCard(c83039729.spfilter,c:GetControler(),LOCATION_MZONE,0,1,nil)
		and not Duel.IsExistingMatchingCard(c83039729.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c83039729.thcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return not c:IsReason(REASON_BATTLE) and rp==1-tp and c:GetPreviousControler()==tp
end
function c83039729.filter(c)
	return c:IsSetCard(0x3d) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c83039729.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c83039729.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c83039729.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c83039729.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c83039729.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
function c83039729.exfilter(c,fid)
	return c:IsFaceup() and c:GetCode()==83039729 and (fid==nil or c:GetFieldID()<fid)
end
function c83039729.excon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c83039729.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c83039729.splimit(e,se,sp,st,spos,tgp)
	if bit.band(spos,POS_FACEDOWN)~=0 then return true end
	return not Duel.IsExistingMatchingCard(c83039729.exfilter,tgp,LOCATION_ONFIELD,0,1,nil)
end
function c83039729.descon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c83039729.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil,c:GetFieldID())
end
