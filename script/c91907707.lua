--クリフォート・アーカイブ
function c91907707.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--splimit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetTargetRange(1,0)
	e2:SetCondition(c91907707.con)
	e2:SetTarget(c91907707.splimit)
	c:RegisterEffect(e2)
	--atk
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_UPDATE_ATTACK)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,0)
	e3:SetCondition(c91907707.con)
	e3:SetTarget(c91907707.atktg)
	e3:SetValue(300)
	c:RegisterEffect(e3)
	--summon with no tribute
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(91907707,0))
	e4:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_SUMMON_PROC)
	e4:SetCondition(c91907707.ntcon)
	c:RegisterEffect(e4)
	--act limit
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e5:SetCode(EVENT_SUMMON_SUCCESS)
	e5:SetOperation(c91907707.regop)
	c:RegisterEffect(e5)
	local e6=e5:Clone()
	e6:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e6)
	--tohand
	local e8=Effect.CreateEffect(c)
	e8:SetDescription(aux.Stringid(91907707,1))
	e8:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e8:SetCategory(CATEGORY_TOHAND)
	e8:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DELAY+EFFECT_FLAG_DAMAGE_STEP)
	e8:SetCode(EVENT_RELEASE)
	e8:SetTarget(c91907707.thtg)
	e8:SetOperation(c91907707.thop)
	c:RegisterEffect(e8)
end
function c91907707.con(e,tp,eg,ep,ev,re,r,rp)
	local seq=e:GetHandler():GetSequence()
	return seq==6 or seq==7
end
function c91907707.splimit(e,c,tp,sumtp,sumpos)
	return not c:IsSetCard(0xaa)
end
function c91907707.atktg(e,c)
	return c:IsSetCard(0xaa)
end
function c91907707.ntcon(e,c)
	if c==nil then return true end
	return c:GetLevel()>4 and Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
end
function c91907707.regop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if bit.band(e:GetHandler():GetSummonType(),SUMMON_TYPE_NORMAL)==SUMMON_TYPE_NORMAL then 
		--immune 	
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_IMMUNE_EFFECT)
		e1:SetReset(RESET_EVENT+0x5ee0000)
		e1:SetValue(c91907707.efilter)
		c:RegisterEffect(e1)
		if c:GetSummonType()==SUMMON_TYPE_ADVANCE then return end
	end
	--search
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e5:SetRange(LOCATION_MZONE)
	e5:SetReset(RESET_EVENT+0x1ff0000)
	e5:SetCode(EFFECT_CHANGE_LEVEL)
	e5:SetValue(4)
	c:RegisterEffect(e5)
	--search
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e6:SetRange(LOCATION_MZONE)
	e6:SetReset(RESET_EVENT+0x1ff0000)
	e6:SetCode(EFFECT_SET_BASE_ATTACK)
	e6:SetValue(1800)
	c:RegisterEffect(e6)
end
function c91907707.efilter(e,te)
	local lv=e:GetHandler():GetLevel()
	return te:IsActiveType(TYPE_MONSTER) 
		and te:IsHasType(EFFECT_TYPE_ACTIONS) 
		and (te:GetHandler():GetOriginalLevel()<lv or te:GetHandler():GetOriginalLevel()<lv)
end
function c91907707.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsAbleToHand() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsAbleToHand,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,Card.IsAbleToHand,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c91907707.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
