--ブリリアント・スパーク
function c3105404.initial_effect(c)
	Duel.EnableGlobalFlag(GLOBALFLAG_DELAYED_QUICKEFFECT)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_DESTROYED)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e1:SetCountLimit(1,3105404+EFFECT_COUNT_CODE_OATH)
	e1:SetCondition(c3105404.condition)
	e1:SetTarget(c3105404.target)
	e1:SetOperation(c3105404.activate)
	c:RegisterEffect(e1)
	--to hand
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetRange(LOCATION_GRAVE)
	e2:SetCost(c3105404.thcost)
	e2:SetTarget(c3105404.thtg)
	e2:SetOperation(c3105404.thop)
	c:RegisterEffect(e2)
end
function c3105404.filter(c,e,tp)
	return c:IsPreviousPosition(POS_FACEUP) and c:GetPreviousControler()==tp and c:IsPreviousLocation(LOCATION_MZONE)
		and (c:IsReason(REASON_EFFECT) or (c:IsReason(REASON_BATTLE) and Duel.GetAttacker():IsControler(1-tp)))
		and c:IsSetCard(0x1047) and c:GetBaseAttack()>0 and c:IsCanBeEffectTarget(e) and c:IsLocation(LOCATION_GRAVE+LOCATION_REMOVED)
end
function c3105404.condition(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp
end
function c3105404.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return eg:IsContains(chkc) and c3105404.filter(chkc,e,tp) end
	if chk==0 then return eg:IsExists(c3105404.filter,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=eg:FilterSelect(tp,c3105404.filter,1,1,nil,e,tp)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,0)
end
function c3105404.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Damage(1-tp,tc:GetBaseAttack(),REASON_EFFECT)
	end
end
function c3105404.cfilter(c)
	return c:IsSetCard(0x1047) and c:IsAbleToGraveAsCost()
end
function c3105404.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(3105404)==0
		and Duel.IsExistingMatchingCard(c3105404.cfilter,tp,LOCATION_HAND,0,1,nil) end
	e:GetHandler():RegisterFlagEffect(3105404,RESET_CHAIN,0,1)
	Duel.DiscardHand(tp,c3105404.cfilter,1,1,REASON_COST,nil)
end
function c3105404.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToHand() end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,e:GetHandler(),1,0,0)
end
function c3105404.thop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SendtoHand(e:GetHandler(),nil,REASON_EFFECT)
	end
end
